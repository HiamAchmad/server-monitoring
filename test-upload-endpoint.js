// Test script untuk endpoint /tugas/upload
const FormData = require('form-data');
const fs = require('fs');
const fetch = require('node-fetch');

async function testUploadEndpoint() {
    console.log('🧪 Testing /tugas/upload endpoint...\n');

    // Buat file dummy untuk test
    const testFilePath = '/tmp/test-upload.txt';
    fs.writeFileSync(testFilePath, 'This is a test file for upload');

    const formData = new FormData();
    formData.append('tugas_id', '1'); // Sesuaikan dengan tugas_id yang ada
    formData.append('pegawai_id', '1'); // Sesuaikan dengan pegawai_id yang ada
    formData.append('catatan', 'Test upload dari script');
    formData.append('file', fs.createReadStream(testFilePath));

    try {
        const response = await fetch('http://localhost:3000/tugas/upload', {
            method: 'POST',
            body: formData
        });

        const result = await response.json();

        console.log('Status:', response.status);
        console.log('Response:', JSON.stringify(result, null, 2));

        // Cleanup
        fs.unlinkSync(testFilePath);

        if (result.success) {
            console.log('\n✅ Test PASSED - Upload endpoint berfungsi!');
        } else {
            console.log('\n❌ Test FAILED - ' + result.message);
        }

    } catch (error) {
        console.error('❌ Error:', error.message);

        // Cleanup on error
        if (fs.existsSync(testFilePath)) {
            fs.unlinkSync(testFilePath);
        }
    }
}

// Juga test endpoint /tugas untuk lihat data
async function testGetTugas() {
    console.log('\n🧪 Testing /tugas endpoint...\n');

    try {
        const response = await fetch('http://localhost:3000/tugas');
        const result = await response.json();

        console.log('Status:', response.status);
        console.log('Success:', result.success);
        console.log('Data count:', result.data ? result.data.length : 0);

        if (result.data && result.data.length > 0) {
            console.log('\nSample tugas:');
            console.log(JSON.stringify(result.data[0], null, 2));
        } else {
            console.log('⚠️  Tidak ada data tugas di database!');
        }

    } catch (error) {
        console.error('❌ Error:', error.message);
    }
}

// Run tests
(async () => {
    await testGetTugas();
    // Uncomment untuk test upload
    // await testUploadEndpoint();
})();
