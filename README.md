# Nimonspoli

Nimonspoli adalah game papan mirip Monopoly berbasis CLI/terminal untuk Tugas Besar 1 IF2010 Pemrograman Berorientasi Objek. Project ini dibuat dengan C++ dan menerapkan konsep OOP untuk memodelkan pemain, papan, petak, properti, kartu, transaksi, serta alur permainan.

Dalam permainan, 2-4 pemain bergerak di papan 40 petak, membeli dan mengelola properti, membayar sewa atau pajak, mengikuti lelang, memakai kartu, membangun rumah/hotel, menyimpan atau memuat permainan, dan bermain sampai kondisi menang tercapai.

## Fitur

- Game CLI interaktif untuk 2-4 pemain.
- Pembacaan konfigurasi dari folder di dalam `config/`, misalnya `config/default/`.
- Papan standar 40 petak dengan street, railroad, utility, action, dan special tile.
- Pergerakan memakai dua dadu enam sisi, termasuk aturan double, extra roll, tiga kali double, penjara, dan gaji GO.
- Street property dapat dibeli, disewakan, digadaikan, ditebus, dilelang, dan di-upgrade dengan rumah/hotel.
- Railroad dan utility otomatis dimiliki pemain pertama yang mendarat di petak tersebut.
- Perhitungan sewa berdasarkan tipe properti, jumlah kepemilikan, level bangunan, monopoli warna, hasil dadu, dan efek festival.
- Petak Kesempatan dan Dana Umum, kartu kemampuan, serta batas kartu di tangan pemain.
- Festival untuk menggandakan sewa properti terpilih selama beberapa giliran.
- Pajak PPH dan PBM.
- Sistem bangkrut, likuidasi aset, pengambilalihan aset, dan pengembalian aset ke bank.
- Penentuan pemenang berdasarkan bankruptcy rule atau max turn rule.
- Save/load permainan dengan file teks terstruktur.
- Transaction logger untuk mencatat event penting selama permainan.

## Kartu

Nimonspoli memiliki dua jenis petak kartu: Kesempatan dan Dana Umum. Efek kartu dapat memindahkan pemain, mengirim pemain ke penjara, memberi hadiah, atau meminta pemain membayar biaya tertentu.

Pemain juga memiliki kartu kemampuan di tangan. Pada awal giliran, pemain menerima kartu kemampuan acak. Pemain dapat menyimpan maksimal 3 kartu, menggunakan maksimal 1 kartu sebelum melempar dadu, lalu kartu yang dipakai masuk ke discard pile. Jika deck habis, discard pile dikocok kembali menjadi deck baru.

Jenis kartu kemampuan meliputi `MoveCard`, `DiscountCard`, `ShieldCard`, `TeleportCard`, `LassoCard`, dan `DemolitionCard`.

## Requirements

- CMake 3.15 atau lebih baru.
- Compiler C++17, misalnya `g++`.
- Linux atau WSL (opsional).

> Catatan: Program ini menargetkan Linux/WSL. Windows dan platform lainnya tidak diuji sehingga tidak bisa dipastikan kelancaran program.

## Build

Jalankan command berikut dari root repository:

```bash
cmake -S . -B build
cmake --build build
```

Executable akan dibuat di:

```bash
bin/nimonspoli
```

## Run

Jalankan executable dari root repository:

```bash
./bin/nimonspoli
```

Saat program meminta folder konfigurasi, masukkan nama folder yang berada di dalam `config/`. Untuk konfigurasi bawaan, masukkan:

```text
default
```

Jadi input yang benar adalah `default`, bukan `config/default`.

Pada awal program, pemain dapat memilih memulai permainan baru atau memuat permainan dari save data.

## Konfigurasi

Folder konfigurasi default tersedia di `config/default/`. Setiap konfigurasi berisi beberapa file teks berikut:

| File | Fungsi |
| --- | --- |
| `property.txt` | Definisi properti: indeks, kode, nama, tipe, warna/grup, harga tanah, nilai gadai, biaya upgrade rumah/hotel, dan nilai sewa level 0-5. |
| `aksi.txt` | Definisi petak aksi/special seperti GO, Dana Umum, Kesempatan, pajak, festival, penjara, bebas parkir, dan go to jail. |
| `railroad.txt` | Tabel sewa railroad berdasarkan jumlah railroad yang dimiliki pemain. |
| `utility.txt` | Multiplier utility berdasarkan jumlah utility yang dimiliki pemain. |
| `tax.txt` | Nilai pajak PPH flat, persentase PPH, dan PBM flat. |
| `special.txt` | Gaji saat melewati/mendarat di GO dan denda keluar penjara. |
| `misc.txt` | `MAX_TURN` dan `SALDO_AWAL` pemain. |

## Save / Load

Save data dibaca dan disimpan sebagai file `.txt` terstruktur di dalam folder `data/`. Pada saat load, pemain cukup memasukkan nama folder atau save data yang diminta program sesuai prompt.

Save file menyimpan state permainan utama, antara lain:

- Turn saat ini dan max turn.
- Jumlah pemain.
- Data pemain dengan format `<USERNAME> <UANG> <POSISI_PETAK> <STATUS>`.
- Status pemain menggunakan nilai `ACTIVE`, `BANKRUPT`, atau `JAILED`.
- Kartu di tangan pemain, termasuk tipe kartu dan nilai tambahan jika ada.
- Urutan pemain dan pemain aktif.
- Status properti, termasuk kode properti, tipe, owner, status, festival multiplier, festival duration, dan jumlah bangunan/status hotel.
- State deck kartu kemampuan.
- State transaction log.

Save hanya dilakukan pada awal giliran pemain sebelum aksi apa pun, termasuk sebelum lempar dadu. Load dilakukan sebelum permainan dimulai.

## Gameplay Commands

| Command | Fungsi | Kapan digunakan |
| --- | --- | --- |
| `CETAK_PAPAN` | Menampilkan papan dan status petak/properti. | Saat prompt giliran pemain. |
| `LEMPAR_DADU` | Melempar dua dadu dan menjalankan efek petak tujuan. | Saat pemain ingin bergerak. |
| `ATUR_DADU X Y` | Mengatur nilai dadu manual, contoh `ATUR_DADU 1 1`. | Untuk demo atau pengujian skenario. |
| `CETAK_AKTA <kode>` | Menampilkan akta/detail properti berdasarkan kode petak. | Saat pemain ingin melihat informasi properti. |
| `CETAK_PROPERTI` | Menampilkan daftar properti milik pemain aktif. | Saat giliran pemain. |
| `GADAI` | Menggadaikan properti milik pemain. | Saat pemain ingin mendapatkan uang dari aset. |
| `TEBUS` | Menebus properti yang sedang digadaikan. | Saat pemain ingin mengaktifkan kembali properti. |
| `BANGUN` | Membangun rumah atau hotel pada street property. | Saat pemain memiliki properti yang dapat di-upgrade. |
| `SIMPAN` | Menyimpan state permainan ke save file. | Di awal giliran sebelum aksi apa pun. |
| `MUAT` | Memuat state permainan dari save file. | Sebelum permainan dimulai. |
| `CETAK_LOG` | Menampilkan log transaksi permainan. | Saat prompt giliran pemain. |
| `GUNAKAN_KEMAMPUAN` | Menggunakan kartu kemampuan dari tangan pemain. | Saat pemain memiliki kartu kemampuan. |

Beberapa aksi dijalankan otomatis oleh sistem saat kondisi tertentu terjadi:

| Aksi otomatis | Pemicu |
| --- | --- |
| `BELI` | Pemain mendarat di street property yang belum dimiliki. |
| `BAYAR_SEWA` | Pemain mendarat di properti milik pemain lain. |
| `BAYAR_PAJAK` | Pemain mendarat di petak pajak. |
| `LELANG` | Properti harus masuk proses lelang. |
| `FESTIVAL` | Pemain mendarat di petak festival. |
| `BANGKRUT DAN AMBIL ALIH ASET` | Pemain tidak dapat memenuhi kewajiban pembayaran. |
| `MENANG` | Kondisi kemenangan tercapai. |
| `KESEMPATAN / DANA UMUM` | Pemain mendarat di petak kartu. |
| `DROP KARTU KEMAMPUAN` | Jumlah kartu kemampuan pemain melebihi batas maksimum. |

## Struktur Proyek

| Path | Isi |
| --- | --- |
| `src/main.cpp` | Entry point program. |
| `src/core/`, `include/core/` | Layer aplikasi dan konfigurasi game. |
| `src/models/`, `include/models/` | Core game logic: board, tile, property, player, card, dice, auction, bank, dan game session. |
| `src/utils/`, `include/utils/` | Utility dan data access: config loader, save/load handler, exception, dan transaction logger. |
| `config/default/` | Konfigurasi bawaan permainan. |
| `data/` | Lokasi save data dan contoh file save. |
| `bin/` | Output executable hasil build. |
| `build/` | Folder hasil konfigurasi dan build CMake. |

Arsitektur project dibagi menjadi beberapa layer:

- User Interaction/UI Layer: input dan output terminal melalui alur aplikasi.
- Game Logic/Core Layer: aturan permainan, perhitungan, state management, dan objek domain.
- Data Access Layer: pembacaan konfigurasi, save/load, dan file I/O.

## Catatan OOP

- `Tile` adalah abstract base class dengan virtual function `onLanded()`. Turunannya mencakup `StartTile`, `PropertyTile`, `TaxTile`, `CardTile`, `FestivalTile`, `JailTile`, `GoToJailTile`, dan `FreeParkingTile`.
- `Property` memiliki turunan `StreetProperty`, `RailroadProperty`, dan `UtilityProperty` untuk membedakan aturan sewa dan perilaku properti.
- `Card` memiliki hierarki `ActionCard` dan `SkillCard`, dengan kartu-kartu turunan untuk efek yang berbeda.
- `Player` menjadi base class untuk pemain dan menyimpan state seperti uang, posisi, properti, status, dan kartu.
- `CardDeck<T>` menunjukkan penggunaan generic/template class.
- STL digunakan untuk menyimpan dan mencari state permainan, terutama `vector`, `map`, dan `unordered_map`.
- Exception handling digunakan untuk menangani input tidak valid, aksi tidak valid, resource file bermasalah, dan kondisi permainan khusus.
- Operator overloading digunakan pada operasi uang dan perbandingan kekayaan pemain.

## Contoh Alur Kerja Program

Build dan jalankan program:

```bash
cmake -S . -B build
cmake --build build
./bin/nimonspoli
```

Contoh alur input demo:

```text
Nama folder konfigurasi: default
Muat permainan lewat save data? N
Masukkan jumlah pemain (2-4): 2
Username pemain 1: Alice
Username pemain 2: Bob
CETAK_PAPAN
ATUR_DADU 1 1
LEMPAR_DADU
CETAK_PROPERTI
CETAK_LOG
SIMPAN
```

Untuk demo load game, pilih opsi load saat awal program dan masukkan save data yang berada di folder `data/`.

## Contributors

- 13524004 / Muhammad Fatih Irkham Mauludi
- 13524060 / Steven Tan
- 13524071 / Kalyca Nathania Benedicta Manullang
- 13524088 / Syaqina Octavia Rizha
- 13524095 / Jingglang Galih Rinenggan

