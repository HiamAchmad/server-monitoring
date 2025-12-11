// ============================================
// ATTENDANCE LOGIC - Absen Masuk/Keluar & Lembur
// ============================================

/**
 * Hitung durasi kerja dalam menit
 * @param {string} waktu_masuk - Format HH:MM:SS
 * @param {string} waktu_keluar - Format HH:MM:SS
 * @returns {number} - Durasi dalam menit
 */
function hitungDurasiKerja(waktu_masuk, waktu_keluar) {
    const [jamMasuk, menitMasuk, detikMasuk] = waktu_masuk.split(':').map(Number);
    const [jamKeluar, menitKeluar, detikKeluar] = waktu_keluar.split(':').map(Number);

    const totalMenitMasuk = jamMasuk * 60 + menitMasuk;
    const totalMenitKeluar = jamKeluar * 60 + menitKeluar;

    let durasi = totalMenitKeluar - totalMenitMasuk;

    // Jika negatif (lewat tengah malam), tambah 24 jam
    if (durasi < 0) {
        durasi += 24 * 60;
    }

    return durasi;
}

/**
 * Hitung durasi lembur berdasarkan jam kerja standar
 * Jam kerja standar: 08:00 - 17:00 (9 jam, dengan istirahat 1 jam = 8 jam kerja efektif)
 * Lembur dihitung jika:
 * - Keluar setelah jam 17:00
 * - Atau total durasi kerja > 8 jam (480 menit)
 *
 * @param {string} waktu_keluar - Format HH:MM:SS
 * @param {number} durasi_kerja - Durasi kerja total dalam menit
 * @returns {object} - {durasi_lembur (menit), status_lembur (Ya/Tidak)}
 */
function hitungLembur(waktu_keluar, durasi_kerja) {
    const [jamKeluar, menitKeluar] = waktu_keluar.split(':').map(Number);

    // Jam pulang standar: 17:00
    const jamPulangStandar = 17;
    const menitPulangStandar = 0;

    // Durasi kerja standar: 8 jam = 480 menit (sudah dikurangi istirahat 1 jam)
    const durasiKerjaStandar = 480; // 8 jam

    let durasi_lembur = 0;
    let status_lembur = 'Tidak';

    // Cek 1: Apakah keluar setelah jam 17:00?
    const totalMenitKeluar = jamKeluar * 60 + menitKeluar;
    const totalMenitPulangStandar = jamPulangStandar * 60 + menitPulangStandar;

    if (totalMenitKeluar > totalMenitPulangStandar) {
        // Lembur berdasarkan waktu keluar
        durasi_lembur = totalMenitKeluar - totalMenitPulangStandar;
        status_lembur = 'Ya';
    }

    // Cek 2: Apakah total durasi kerja > 8 jam?
    // (Ini untuk kasus masuk lebih awal dan keluar tepat waktu tapi total > 8 jam)
    if (durasi_kerja > durasiKerjaStandar) {
        const lemburDariDurasi = durasi_kerja - durasiKerjaStandar;

        // Ambil yang lebih besar
        if (lemburDariDurasi > durasi_lembur) {
            durasi_lembur = lemburDariDurasi;
        }

        status_lembur = 'Ya';
    }

    return {
        durasi_lembur: Math.round(durasi_lembur),
        status_lembur
    };
}

/**
 * Format durasi menit ke format jam:menit
 * @param {number} menit - Durasi dalam menit
 * @returns {string} - Format HH:MM
 */
function formatDurasi(menit) {
    const jam = Math.floor(menit / 60);
    const sisa_menit = menit % 60;
    return `${String(jam).padStart(2, '0')}:${String(sisa_menit).padStart(2, '0')}`;
}

/**
 * Cek apakah karyawan sudah absen masuk hari ini
 * @param {object} db - Database connection pool
 * @param {number} pegawai_id - ID pegawai
 * @param {function} callback - Callback(err, result)
 */
function cekAbsenMasukHariIni(db, pegawai_id, callback) {
    const query = `
        SELECT * FROM absensi
        WHERE pegawai_id = $1
        AND DATE(timestamp) = CURRENT_DATE
        AND tipe_absen = 'Masuk'
        AND waktu_keluar IS NULL
        ORDER BY timestamp DESC
        LIMIT 1
    `;

    db.query(query, [pegawai_id], callback);
}

/**
 * Handle absen masuk
 * @param {object} db - Database connection pool
 * @param {object} data - {pegawai_id, waktu_absen, keterangan}
 * @param {function} callback - Callback(err, result)
 */
function handleAbsenMasuk(db, data, callback) {
    const { pegawai_id, waktu_absen, keterangan } = data;

    const query = `
        INSERT INTO absensi (
            pegawai_id,
            waktu_absen,
            keterangan,
            tipe_absen,
            timestamp
        )
        VALUES ($1, $2, $3, 'Masuk', NOW())
        RETURNING id_absensi
    `;

    db.query(query, [pegawai_id, waktu_absen, keterangan || 'Hadir'], callback);
}

/**
 * Handle absen keluar
 * @param {object} db - Database connection pool
 * @param {object} data - {pegawai_id, waktu_keluar}
 * @param {function} callback - Callback(err, result)
 */
function handleAbsenKeluar(db, data, callback) {
    const { pegawai_id, waktu_keluar } = data;

    // Cari record absen masuk hari ini
    cekAbsenMasukHariIni(db, pegawai_id, (err, result) => {
        if (err) {
            return callback(err);
        }

        if (result.rows.length === 0) {
            return callback(new Error('Tidak ada record absen masuk hari ini'));
        }

        const absenMasuk = result.rows[0];
        const waktu_masuk = absenMasuk.waktu_absen;

        // Hitung durasi kerja
        const durasi_kerja = hitungDurasiKerja(waktu_masuk, waktu_keluar);

        // Hitung lembur
        const { durasi_lembur, status_lembur } = hitungLembur(waktu_keluar, durasi_kerja);

        // Update record
        const updateQuery = `
            UPDATE absensi
            SET waktu_keluar = $1,
                durasi_kerja = $2,
                durasi_lembur = $3,
                status_lembur = $4
            WHERE id_absensi = $5
            RETURNING *
        `;

        db.query(
            updateQuery,
            [waktu_keluar, durasi_kerja, durasi_lembur, status_lembur, absenMasuk.id_absensi],
            callback
        );
    });
}

module.exports = {
    hitungDurasiKerja,
    hitungLembur,
    formatDurasi,
    cekAbsenMasukHariIni,
    handleAbsenMasuk,
    handleAbsenKeluar
};
