-- Reset data karyawan dan buat ulang dari awal
-- Data karyawan sesuai permintaan user

BEGIN;

-- Hapus semua data absensi (foreign key ke pegawai)
DELETE FROM absensi;

-- Hapus semua data tugas (foreign key ke pegawai)
DELETE FROM tugas;

-- Hapus semua data pegawai
DELETE FROM pegawai;

-- Reset sequence ID pegawai
ALTER SEQUENCE pegawai_id_pegawai_seq RESTART WITH 1;

-- Insert data karyawan baru
INSERT INTO pegawai (nip, nama_pegawai, posisi, email, no_telepon, tanggal_bergabung, status, fingerprint_id) VALUES
('20210410700073', 'Achmad Hasan', 'Manager', 'achmad.hasan@company.com', '081234567890', '2021-01-15', 'aktif', NULL),
('20210410700086', 'Andika Restu Ramadhan', 'Staff', 'andika.restu@company.com', '081234567891', '2021-02-01', 'aktif', NULL),
('20210410700052', 'Luthfi Hermawan', 'Staff', 'luthfi.hermawan@company.com', '081234567892', '2021-03-10', 'aktif', NULL);

COMMIT;

-- Tampilkan data pegawai baru
SELECT id_pegawai, nip, nama_pegawai, posisi, fingerprint_id, status FROM pegawai ORDER BY id_pegawai;
