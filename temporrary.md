# Dekomposisi Class Permainan Nimonspoli

## Pendekatan Dekomposisi

Dekomposisi dilakukan dengan mengidentifikasi **entitas dunia nyata** dalam permainan, lalu mengelompokkannya berdasarkan tanggung jawab sesuai prinsip Single Responsibility dan Layered Architecture (UI → Game Logic → Data Access).

**Langkah berpikir:**

1. **Identifikasi noun (kata benda)** dari spesifikasi: Pemain, Papan, Petak, Properti, Dadu, Kartu, Akta, Bank, Lelang, Festival, Log, Game.
2. **Kelompokkan berdasarkan perilaku**: Petak punya banyak varian (Lahan, Stasiun, Utilitas, Pajak, Penjara, dll.) → gunakan **inheritance**. Kartu juga bervarian → inheritance. Properti dimiliki pemain → **asosiasi/agregasi**.
3. **Pisahkan concern**: Tampilan papan ≠ logika papan. Baca/tulis file ≠ logika game. Maka dibuat layer terpisah.
4. **Identifikasi generic**: Deck kartu berperilaku sama untuk berbagai jenis kartu → **generic class** `CardDeck<T>`.

---

## Daftar Class Lengkap

---

### 1. `Game`

**Layer:** Game Logic (Core)

**Justifikasi:** Menjadi orchestrator utama yang mengelola alur permainan (giliran, kondisi menang, transisi state). Tanpa class ini, logika permainan tersebar tanpa koordinasi.

**Atribut:**
- `board: Board*` — papan permainan
- `players: vector<Player*>` — daftar pemain
- `turnOrder: vector<int>` — urutan giliran
- `currentTurnIndex: int` — indeks pemain yang sedang bermain
- `currentTurn: int` — nomor turn saat ini
- `maxTurn: int` — batas maksimum turn
- `startingBalance: int` — saldo awal per pemain
- `dice: Dice*` — objek dadu
- `chanceDeck: CardDeck<ActionCard*>*` — tumpukan kartu kesempatan
- `communityDeck: CardDeck<ActionCard*>*` — tumpukan kartu dana umum
- `skillDeck: CardDeck<SkillCard*>*` — tumpukan kartu kemampuan
- `logger: TransactionLogger*` — pencatat transaksi
- `isRunning: bool` — status game berjalan
- `hasRolledDice: bool` — apakah pemain sudah lempar dadu di giliran ini
- `hasUsedSkill: bool` — apakah pemain sudah pakai kartu kemampuan di giliran ini
- `consecutiveDoubles: int` — jumlah double berturut-turut

**Method:**
- `start()` — memulai permainan baru
- `loadGame(filename: string)` — memuat dari file
- `nextTurn()` — pindah ke giliran berikutnya
- `getCurrentPlayer(): Player*` — pemain yang sedang bermain
- `processCommand(command: string)` — parsing dan eksekusi perintah
- `checkWinCondition(): bool` — cek apakah game selesai
- `handleBankruptcy(debtor: Player*, creditor: Player*, amount: int)` — proses bangkrut
- `handleAuction(property: Property*, triggeredBy: Player*)` — mulai lelang
- `distributeSalary(player: Player*)` — beri gaji saat lewat GO
- `removePlayer(player: Player*)` — keluarkan pemain bangkrut
- `endGame()` — akhiri permainan dan tampilkan pemenang

**Hubungan:**
- **Komposisi** dengan `Board`, `Dice`, `TransactionLogger`
- **Agregasi** dengan `Player`, `CardDeck`

---

### 2. `Board`

**Layer:** Game Logic

**Justifikasi:** Papan permainan adalah entitas tersendiri yang menyimpan susunan petak. Dipisahkan dari Game agar mendukung bonus "Papan Dinamis" — susunan petak bisa berubah tanpa mengubah logika game.

**Atribut:**
- `tiles: vector<Tile*>` — daftar petak berurutan
- `tileMap: map<string, Tile*>` — mapping kode petak → objek petak
- `totalTiles: int` — jumlah total petak

**Method:**
- `getTile(index: int): Tile*` — ambil petak berdasarkan indeks
- `getTileByCode(code: string): Tile*` — ambil petak berdasarkan kode
- `getNextTileIndex(current: int, steps: int): int` — hitung posisi setelah maju
- `findNearestStation(fromIndex: int): int` — cari stasiun terdekat
- `getJailIndex(): int` — indeks petak penjara
- `getGoIndex(): int` — indeks petak GO
- `print(players: vector<Player*>, turn: int, maxTurn: int)` — cetak papan

**Hubungan:**
- **Komposisi** dengan `Tile` (Board memiliki dan mengelola semua Tile)

---

### 3. `Tile` (Abstract Class)

**Layer:** Game Logic

**Justifikasi:** Semua petak memiliki kesamaan (nama, kode, indeks) tetapi berbeda perilaku saat pemain mendarat. Menjadikannya abstract memungkinkan polymorphism via `onLanded()`.

**Atribut:**
- `index: int` — posisi di papan
- `code: string` — kode unik (e.g., "JKT")
- `name: string` — nama lengkap
- `category: string` — kategori warna untuk tampilan

**Method (Virtual/Abstract):**
- `onLanded(player: Player*, game: Game*) = 0` — aksi saat pemain mendarat (pure virtual)
- `getDisplayInfo(): string` — info untuk ditampilkan di papan
- `getType(): string` — jenis petak

**Subclass (Inheritance):**

---

### 3a. `PropertyTile` (Abstract, extends `Tile`)

**Justifikasi:** Street, Railroad, dan Utility sama-sama properti yang bisa dimiliki, digadai, dan disewakan, tetapi cara perolehan dan perhitungan sewanya berbeda.

**Atribut:**
- `property: Property*` — referensi ke data properti

**Method:**
- `onLanded(player: Player*, game: Game*)` — cek kepemilikan, trigger beli/sewa/lelang
- `getProperty(): Property*`

**Subclass:**

#### 3a-i. `StreetTile` (extends `PropertyTile`)
- Override `onLanded()`: tawarkan beli atau lelang jika belum dimiliki; bayar sewa jika milik orang lain.

#### 3a-ii. `RailroadTile` (extends `PropertyTile`)
- Override `onLanded()`: otomatis dimiliki pemain pertama yang mendarat; bayar sewa berdasarkan jumlah railroad yang dimiliki pemilik.

#### 3a-iii. `UtilityTile` (extends `PropertyTile`)
- Override `onLanded()`: otomatis dimiliki pemain pertama; sewa = total dadu × faktor pengali.

---

### 3b. `TaxTile` (extends `Tile`)

**Justifikasi:** Petak pajak punya mekanisme unik (PPH dengan 2 opsi, PBM flat). Dipisahkan karena bukan properti dan tidak bisa dimiliki.

**Atribut:**
- `taxType: string` — "PPH" atau "PBM"
- `flatAmount: int` — nilai pajak flat
- `percentage: int` — persentase (khusus PPH)

**Method:**
- `onLanded(player: Player*, game: Game*)` — hitung dan potong pajak

---

### 3c. `CardTile` (extends `Tile`)

**Justifikasi:** Petak Kesempatan dan Dana Umum memicu pengambilan kartu dari deck yang berbeda.

**Atribut:**
- `cardType: string` — "KESEMPATAN" atau "DANA_UMUM"

**Method:**
- `onLanded(player: Player*, game: Game*)` — ambil kartu dari deck, jalankan efek

---

### 3d. `FestivalTile` (extends `Tile`)

**Justifikasi:** Mekanisme festival unik (pilih properti, gandakan sewa, durasi, stack hingga 3×). Perlu class sendiri.

**Method:**
- `onLanded(player: Player*, game: Game*)` — tampilkan properti pemain, proses pemilihan dan penggandaan sewa

---

### 3e. `GoTile` (extends `Tile`)

**Method:**
- `onLanded(player: Player*, game: Game*)` — beri gaji (jika belum diberikan saat lewat)

---

### 3f. `JailTile` (extends `Tile`)

**Atribut:**
- `jailFine: int` — denda keluar penjara
- `inmates: vector<Player*>` — pemain yang dipenjara
- `visitors: vector<Player*>` — pemain yang mampir
- `jailTurns: map<Player*, int>` — jumlah giliran di penjara per pemain

**Method:**
- `onLanded(player: Player*, game: Game*)` — tandai sebagai pengunjung
- `imprisonPlayer(player: Player*)` — masukkan ke penjara
- `handleJailTurn(player: Player*, game: Game*)` — pilihan: bayar/kartu/lempar double
- `releasePlayer(player: Player*)` — bebaskan
- `isInmate(player: Player*): bool`
- `getJailTurns(player: Player*): int`

---

### 3g. `GoToJailTile` (extends `Tile`)

**Method:**
- `onLanded(player: Player*, game: Game*)` — pindahkan pemain ke penjara tanpa gaji GO

---

### 3h. `FreeParkingTile` (extends `Tile`)

**Method:**
- `onLanded(player: Player*, game: Game*)` — tidak melakukan apa-apa

---

### 4. `Property`

**Layer:** Game Logic

**Justifikasi:** Data kepemilikan properti (harga, status, pemilik, bangunan, efek festival) perlu dipisahkan dari Tile karena Property bisa diperjualbelikan, digadai, dan berubah state secara independen dari posisinya di papan.

**Atribut:**
- `id: int` — ID properti
- `code: string` — kode unik
- `name: string` — nama properti
- `type: string` — "STREET", "RAILROAD", atau "UTILITY"
- `colorGroup: string` — warna grup (khusus street)
- `price: int` — harga beli
- `mortgageValue: int` — nilai gadai
- `housePrice: int` — harga bangun rumah (street)
- `hotelPrice: int` — harga upgrade hotel (street)
- `rentLevels: vector<int>` — sewa per level bangunan (street, 6 level: 0-5)
- `owner: Player*` — pemilik (nullptr jika bank)
- `status: PropertyStatus` — enum: BANK, OWNED, MORTGAGED
- `buildingCount: int` — jumlah bangunan (0-4 rumah, 5=hotel)
- `festivalMultiplier: int` — pengali festival (1, 2, 4, atau 8)
- `festivalDuration: int` — sisa durasi festival

**Method:**
- `calculateRent(diceTotal: int, ownedCountOfType: int): int` — hitung sewa berdasarkan jenis
- `canBuild(): bool` — cek apakah bisa dibangun
- `build()` — tambah 1 rumah atau upgrade ke hotel
- `sellBuilding(): int` — jual 1 bangunan, return uang
- `mortgage(): int` — gadaikan, return nilai gadai
- `unmortgage(): int` — tebus, return biaya tebus (harga beli penuh)
- `applyFestival()` — gandakan sewa
- `decrementFestival()` — kurangi durasi festival 1
- `resetFestival()` — reset efek festival
- `getTotalValue(): int` — harga beli + harga bangunan
- `getSellValue(): int` — nilai jual ke bank (harga beli + ½ harga bangunan)
- `isMonopolized(): bool` — cek apakah seluruh color group dimiliki pemilik yang sama
- `printCertificate()` — cetak akta kepemilikan

**Hubungan:**
- **Asosiasi** dengan `Player` (pemilik)
- Digunakan oleh `PropertyTile`

---

### 5. `Player`

**Layer:** Game Logic

**Justifikasi:** Pemain adalah entitas inti yang memiliki uang, posisi, properti, kartu, dan status. Harus menjadi class sendiri karena menyimpan state yang kompleks dan berinteraksi dengan hampir semua komponen lain.

**Atribut:**
- `username: string` — nama pemain
- `money: int` — uang tunai
- `position: int` — indeks petak saat ini
- `status: PlayerStatus` — enum: ACTIVE, BANKRUPT, JAILED
- `properties: vector<Property*>` — properti yang dimiliki
- `hand: vector<SkillCard*>` — kartu kemampuan di tangan (max 3)
- `playerIndex: int` — nomor urut pemain (untuk tampilan P1-P4)
- `shieldActive: bool` — apakah shield sedang aktif
- `discountPercent: int` — persentase diskon aktif
- `jailAttempts: int` — percobaan lempar dadu di penjara

**Method:**
- `operator+(amount: int): Player&` — tambah uang
- `operator-(amount: int): Player&` — kurangi uang
- `operator+=(amount: int): Player&` — tambah uang (compound)
- `operator-=(amount: int): Player&` — kurangi uang (compound)
- `operator<(other: Player&): bool` — bandingkan kekayaan
- `operator>(other: Player&): bool` — bandingkan kekayaan
- `moveTo(position: int)` — pindah ke posisi tertentu
- `moveForward(steps: int, board: Board*): int` — maju sejumlah langkah, return posisi baru
- `addProperty(property: Property*)` — tambah properti
- `removeProperty(property: Property*)` — hapus properti
- `addCard(card: SkillCard*)` — tambah kartu ke tangan
- `removeCard(index: int): SkillCard*` — buang kartu
- `getCardCount(): int`
- `getTotalWealth(): int` — uang + harga beli semua properti + harga beli semua bangunan
- `getPropertiesByColor(color: string): vector<Property*>` — properti berdasarkan warna
- `getOwnedRailroadCount(): int`
- `getOwnedUtilityCount(): int`
- `canAfford(amount: int): bool`
- `getMaxLiquidationValue(): int` — estimasi dana maksimal dari likuidasi
- `printProperties()` — cetak daftar properti
- `isActive(): bool`
- `isBankrupt(): bool`
- `isJailed(): bool`

**Hubungan:**
- **Agregasi** dengan `Property` (memiliki tapi Property bisa berpindah pemilik)
- **Agregasi** dengan `SkillCard`

---

### 6. `Dice`

**Layer:** Game Logic

**Justifikasi:** Dadu dienkapsulasi karena punya dua mode (random dan manual) dan perlu menyimpan hasil terakhir untuk kalkulasi Utility dan deteksi double.

**Atribut:**
- `die1: int` — nilai dadu 1
- `die2: int` — nilai dadu 2

**Method:**
- `roll()` — lempar random
- `setManual(x: int, y: int)` — atur manual
- `getTotal(): int` — die1 + die2
- `isDouble(): bool` — die1 == die2
- `getDie1(): int`
- `getDie2(): int`

---

### 7. `ActionCard` (Abstract Class)

**Layer:** Game Logic

**Justifikasi:** Kartu Kesempatan dan Dana Umum memiliki efek yang berbeda-beda tetapi semuanya "diambil dan dijalankan". Abstract class memungkinkan polymorphism.

**Atribut:**
- `description: string` — teks kartu

**Method (Virtual):**
- `execute(player: Player*, game: Game*) = 0` — jalankan efek kartu

**Subclass:**

#### 7a. `ChanceGoToStation` (extends `ActionCard`)
- Pindah ke stasiun terdekat.

#### 7b. `ChanceMoveBack3` (extends `ActionCard`)
- Mundur 3 petak.

#### 7c. `ChanceGoToJail` (extends `ActionCard`)
- Masuk penjara.

#### 7d. `CommunityBirthday` (extends `ActionCard`)
- Terima M100 dari setiap pemain.

#### 7e. `CommunityDoctorFee` (extends `ActionCard`)
- Bayar M700 ke bank.

#### 7f. `CommunityElection` (extends `ActionCard`)
- Bayar M200 ke setiap pemain.

---

### 8. `SkillCard` (Abstract Class)

**Layer:** Game Logic

**Justifikasi:** Kartu Kemampuan Spesial bervariasi (Move, Discount, Shield, Teleport, Lasso, Demolition) tetapi semuanya "digunakan sebelum lempar dadu dan sekali pakai". Polymorphism via `use()`.

**Atribut:**
- `name: string` — nama kartu
- `value: int` — parameter numerik (langkah untuk Move, persen untuk Discount; 0 jika tidak relevan)
- `duration: int` — sisa durasi (khusus Discount; 0 untuk lainnya)

**Method (Virtual):**
- `use(player: Player*, game: Game*) = 0` — aktifkan efek
- `getDescription(): string` — deskripsi untuk ditampilkan
- `getType(): string` — jenis kartu
- `clone(): SkillCard*` — untuk membuat salinan saat diperlukan

**Subclass:**

#### 8a. `MoveCard` (extends `SkillCard`)
- `value` = jumlah langkah maju (random saat didapat)
- `use()`: majukan pemain sejumlah `value` langkah

#### 8b. `DiscountCard` (extends `SkillCard`)
- `value` = persentase diskon (random saat didapat)
- `duration` = 1 giliran
- `use()`: aktifkan diskon pada pemain

#### 8c. `ShieldCard` (extends `SkillCard`)
- `use()`: aktifkan shield pada pemain selama 1 giliran

#### 8d. `TeleportCard` (extends `SkillCard`)
- `use()`: minta input petak tujuan, pindahkan pemain

#### 8e. `LassoCard` (extends `SkillCard`)
- `use()`: pilih lawan di depan, tarik ke petak pemain

#### 8f. `DemolitionCard` (extends `SkillCard`)
- `use()`: pilih properti lawan, hancurkan 1 level bangunan

---

### 9. `CardDeck<T>` (Generic/Template Class)

**Layer:** Game Logic

**Justifikasi:** Tumpukan kartu (Kesempatan, Dana Umum, Kemampuan) semuanya berperilaku sama: draw dari atas, shuffle, reshuffle dari discard pile. Generic class menghindari duplikasi kode.

**Atribut:**
- `drawPile: vector<T>` — tumpukan ambil
- `discardPile: vector<T>` — tumpukan buang

**Method:**
- `draw(): T` — ambil kartu teratas; jika habis, reshuffle discard pile
- `discard(card: T)` — buang kartu ke discard pile
- `shuffle()` — kocok draw pile
- `isEmpty(): bool`
- `size(): int`
- `getDrawPile(): vector<T>` — untuk save/load
- `setDrawPile(cards: vector<T>)` — untuk load

---

### 10. `Auction`

**Layer:** Game Logic

**Justifikasi:** Lelang memiliki alur tersendiri (giliran bid, pass counter, penentuan pemenang) yang cukup kompleks untuk dipisahkan dari Game.

**Atribut:**
- `property: Property*` — properti yang dilelang
- `participants: vector<Player*>` — peserta lelang
- `currentBidder: int` — indeks peserta saat ini
- `highestBid: int` — penawaran tertinggi
- `highestBidder: Player*` — penawar tertinggi
- `consecutivePasses: int` — jumlah pass berturut-turut
- `triggeredBy: Player*` — pemain yang memicu lelang

**Method:**
- `start()` — mulai proses lelang
- `bid(player: Player*, amount: int): bool` — ajukan penawaran
- `pass(player: Player*)` — lewati giliran
- `isFinished(): bool` — cek apakah lelang selesai
- `getWinner(): Player*` — pemenang
- `getFinalPrice(): int` — harga akhir

---

### 11. `TransactionLogger`

**Layer:** Game Logic

**Justifikasi:** Pencatatan log adalah concern terpisah dari logika game. Menyimpan riwayat transaksi untuk ditampilkan dan disave.

**Atribut:**
- `entries: vector<LogEntry>` — semua entri log

**Method:**
- `log(turn: int, username: string, actionType: string, detail: string)` — catat entri baru
- `printAll()` — cetak semua log
- `printLast(n: int)` — cetak n entri terakhir
- `getEntries(): vector<LogEntry>` — untuk save
- `loadEntries(entries: vector<LogEntry>)` — untuk load
- `size(): int`

**Inner Class / Struct-like Class:**

### 11a. `LogEntry`
- `turn: int`
- `username: string`
- `actionType: string`
- `detail: string`
- `toString(): string`

---

### 12. `ConfigLoader`

**Layer:** Data Access

**Justifikasi:** Semua pembacaan file konfigurasi (property.txt, railroad.txt, utility.txt, tax.txt, special.txt, misc.txt) dipusatkan di sini sesuai prinsip Single Responsibility dan layered architecture.

**Atribut:**
- `configPath: string` — path direktori konfigurasi

**Method:**
- `loadProperties(filename: string): vector<Property*>` — baca property.txt
- `loadRailroadRent(filename: string): map<int, int>` — baca railroad.txt (jumlah → sewa)
- `loadUtilityMultiplier(filename: string): map<int, int>` — baca utility.txt (jumlah → pengali)
- `loadTaxConfig(filename: string): TaxConfig` — baca tax.txt
- `loadSpecialConfig(filename: string): SpecialConfig` — baca special.txt
- `loadMiscConfig(filename: string): MiscConfig` — baca misc.txt
- `loadBoardLayout(filename: string): vector<TileConfig>` — untuk bonus papan dinamis

**Data Class pelengkap:**
- `TaxConfig` — { pphFlat, pphPercent, pbmFlat }
- `SpecialConfig` — { goSalary, jailFine }
- `MiscConfig` — { maxTurn, startingBalance }

---

### 13. `SaveLoadManager`

**Layer:** Data Access

**Justifikasi:** Serialisasi dan deserialisasi state game ke/dari file .txt adalah tanggung jawab tersendiri yang tidak boleh dicampur dengan logika game.

**Method:**
- `saveGame(game: Game*, filename: string): bool` — simpan seluruh state ke file
- `loadGame(filename: string, configLoader: ConfigLoader*): Game*` — muat state dari file
- `fileExists(filename: string): bool` — cek apakah file ada

---

### 14. `GameUI`

**Layer:** User Interaction (UI)

**Justifikasi:** Semua input/output terminal dipisahkan dari logika game. Jika nanti dibuat GUI (bonus), cukup ganti layer ini tanpa mengubah core logic.

**Method:**
- `showMainMenu(): int` — tampilkan menu utama (New/Load), return pilihan
- `getPlayerCount(): int` — input jumlah pemain
- `getPlayerName(index: int): string` — input nama pemain
- `showBoard(board: Board*, players: vector<Player*>, turn: int, maxTurn: int)` — cetak papan
- `showCertificate(property: Property*)` — cetak akta
- `showPlayerProperties(player: Player*)` — cetak daftar properti pemain
- `showBuyPrompt(property: Property*, player: Player*): bool` — tanya beli/tidak
- `showRentPayment(payer: Player*, owner: Player*, property: Property*, rent: int)` — info sewa
- `showTaxOptions(player: Player*): int` — pilihan PPH
- `showAuctionPrompt(player: Player*): pair<string, int>` — input BID/PASS
- `showFestivalPrompt(player: Player*): string` — pilih properti festival
- `showMortgageMenu(player: Player*): int` — pilih properti gadai
- `showRedeemMenu(player: Player*): int` — pilih properti tebus
- `showBuildMenu(player: Player*): pair<int, int>` — pilih color group dan petak
- `showLiquidationPanel(player: Player*, obligation: int)` — panel likuidasi
- `showSkillCardMenu(player: Player*): int` — pilih kartu kemampuan
- `showDropCardMenu(player: Player*): int` — pilih kartu dibuang
- `showLog(entries: vector<LogEntry>, count: int)` — cetak log
- `showWinner(winners: vector<Player*>, reason: string)` — tampilkan pemenang
- `showMessage(message: string)` — pesan umum
- `getCommand(): string` — baca input perintah
- `showJailOptions(player: Player*): int` — pilihan di penjara
- `showSavePrompt(): string` — input nama file simpan

---

## Ringkasan Hubungan Antar Class

### Inheritance (IS-A)

```
Tile (abstract)
├── PropertyTile (abstract)
│   ├── StreetTile
│   ├── RailroadTile
│   └── UtilityTile
├── TaxTile
├── CardTile
├── FestivalTile
├── GoTile
├── JailTile
├── GoToJailTile
└── FreeParkingTile

ActionCard (abstract)
├── ChanceGoToStation
├── ChanceMoveBack3
├── ChanceGoToJail
├── CommunityBirthday
├── CommunityDoctorFee
└── CommunityElection

SkillCard (abstract)
├── MoveCard
├── DiscountCard
├── ShieldCard
├── TeleportCard
├── LassoCard
└── DemolitionCard
```

### Komposisi (HAS-A, lifetime terikat)

| Owner | Component | Penjelasan |
|-------|-----------|------------|
| Game | Board | Board tidak ada tanpa Game |
| Game | Dice | Dice hanya ada di konteks Game |
| Game | TransactionLogger | Logger hidup selama game hidup |
| Board | Tile* (semua) | Tile dihancurkan bersama Board |

### Agregasi (HAS-A, lifetime independen)

| Owner | Component | Penjelasan |
|-------|-----------|------------|
| Game | Player* | Player bisa keluar (bangkrut) tanpa Game hancur |
| Game | CardDeck\<ActionCard*\> | Deck bisa di-reshuffle independen |
| Game | CardDeck\<SkillCard*\> | Idem |
| Player | Property* | Properti bisa berpindah pemilik |
| Player | SkillCard* | Kartu bisa dibuang/dipakai |
| PropertyTile | Property* | Tile mereferensi Property, tapi Property punya lifecycle sendiri |
| Auction | Player*, Property* | Lelang bersifat sementara |

### Asosiasi (USES-A)

| Class | Uses | Penjelasan |
|-------|------|------------|
| GameUI | Game, Board, Player, Property | UI membaca state untuk ditampilkan |
| ConfigLoader | Property, TaxConfig, dll. | Membuat objek dari file |
| SaveLoadManager | Game (seluruh state) | Serialisasi/deserialisasi |
| Tile subclasses | Player, Game | Dipanggil saat `onLanded()` |
| ActionCard subclasses | Player, Game | Dipanggil saat `execute()` |
| SkillCard subclasses | Player, Game | Dipanggil saat `use()` |

---

## Diagram Layered Architecture

```
┌─────────────────────────────────┐
│     User Interaction Layer      │
│          (GameUI)               │
└────────────┬────────────────────┘
             │ memanggil / membaca state
┌────────────▼────────────────────┐
│       Game Logic Layer          │
│  Game, Board, Tile*, Player,    │
│  Property, Dice, Auction,       │
│  CardDeck<T>, ActionCard*,      │
│  SkillCard*, TransactionLogger  │
└────────────┬────────────────────┘
             │ baca/tulis file
┌────────────▼────────────────────┐
│      Data Access Layer          │
│  ConfigLoader, SaveLoadManager  │
└─────────────────────────────────┘
```

---

## Checklist Ketentuan Teknis

| Requirement | Implementasi |
|-------------|-------------|
| Inheritance & Polymorphism | Tile → subclass, ActionCard → subclass, SkillCard → subclass |
| Exception Handling | Validasi uang, slot kartu penuh, input tidak valid |
| Operator Overloading | Player: `+`, `-`, `+=`, `-=`, `<`, `>` |
| Function Overloading | Contoh: `CardDeck::draw()` tanpa parameter vs `draw(count)` |
| Abstract Class & Virtual Function | Tile, ActionCard, SkillCard dengan pure virtual |
| Generic Class | `CardDeck<T>` untuk semua jenis deck |
| STL: vector | Player list, Tile list, property list, hand, log entries |
| STL: map | tileMap, railroad rent table, utility multiplier, jail turns |
