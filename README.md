# Soal Shift Modul 3 (Kelompok E6)

###### Nama Anggota :
1. Ifta Jihan Nabila (05111740000034)
2. Komang Yogananda MW (05111740000114)

**Penjelasan Soal Shift Modul 3 Sistem Operasi 2019:**
* [Soal 1](#Soal-1)
* [Soal 2](#Soal-2)
* [Soal 3](#Soal-3)
* [Soal 4](#Soal-4)
* [Soal 5](#Soal-5)

## Soal-1
Semua nama file dan folder harus terenkripsi
Enkripsi yang Atta inginkan sangat sederhana, yaitu Caesar cipher. Namun, Kusuma mengatakan enkripsi tersebut sangat mudah dipecahkan. Dia menyarankan untuk character list diekspansi,tidak hanya alfabet, dan diacak. Berikut character list yang dipakai:

```
qE1~ YMUR2"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\8s;g<{3.u*W-0
```
      
**_Jawaban:_** 

**_Hasil_**  


## Soal-2
Semua file video yang tersimpan secara terpecah-pecah (splitted) harus secara otomatis tergabung (joined) dan diletakkan dalam folder “Videos”
Urutan operasi dari kebutuhan ini adalah:
1.  Tepat saat sebelum file system di-mount
    - Secara otomatis folder “Videos” terbuat di root directory file system
    - Misal ada sekumpulan file pecahan video bernama “computer.mkv.000”, “computer.mkv.001”, “computer.mkv.002”, dst. Maka secara otomatis file pecahan tersebut akan di-join menjadi file video “computer.mkv”. Untuk mempermudah kalian, dipastikan hanya video file saja yang terpecah menjadi beberapa file. File pecahan tersebut dijamin terletak di root folder fuse
    - Karena mungkin file video sangat banyak sehingga mungkin saja saat menggabungkan file video, file system akan membutuhkan waktu yang lama untuk sukses ter-mount. Maka pastikan saat akan menggabungkan file pecahan video, file system akan membuat 1 thread/proses(fork) baru yang dikhususkan untuk menggabungkan file video tersebut
    - Pindahkan seluruh file video yang sudah ter-join ke dalam folder “Videos”
    - Jangan tampilkan file pecahan di direktori manapun
2. Tepat saat file system akan di-unmount
    - Hapus semua file video yang berada di folder “Videos”, tapi jangan hapus file pecahan yang terdapat di root directory file system
    - Hapus folder “Videos” 

**_Jawaban_**  

Pertama-tama operasi fuse **init** harus diimplementasikan sendiri pada program fuse untuk melakukan permintaan Tepat saat sebelum file system di mount.  
Implementasi **init** dapat dilihat dibawah.
```c
static void xmp_init(struct fuse_conn_info *conn){
	char videos[1000] = "Videos";
	enc(videos);
	enc(youtuber);
	enc(filemiris);
	enc(iz1);
	sprintf(videosPath, "%s/%s", dirpath, videos);
	mkdir(videosPath, 0777);
	counter = 0;
	DIR* dp;
	struct dirent *de;
	dp = opendir(dirpath);
	while((de = readdir(dp)) != NULL){
		char temp[1000];
		sprintf(temp, "%s", de->d_name);
		dec(temp);
		printf("init temp %s\n", temp);
		if (isVideo(temp)){
			char namadepan[1000];
			int len = strlen(temp);
			sprintf(namadepan, "%s", temp);
			namadepan[len - 4] = '\0';
			pthread_create(&(buruhVideo[counter++]), NULL, &gabungVideo, (void *)namadepan);
		}
	}
	printf("%d\n", counter);
	return;
}
```
Pada fungsi init terdapat beberapa bagian penting dalam memenuhi syarat **Tepat saat sebelum file system di mount**:  
1. `dp = opendir(dirpath)` yaitu membuka directory tempat pecahan video berada
2. `isVideo(temp)` digunakan untuk memeriksa apakah sebuah file merupakan pecahan video awal bernomor 000. Implementasi fungsi **isVideo** akan dijelaskan dibawah.
3. `pthread_create(&(buruhVideo[counter++]), NULL, &gabungVideo, (void *)namadepan)` apabila sebuah file dinyatakan sebagai sebuah awalan pecahan video, maka nama depan dari file tersebut (contoh video1.mkv.000, namadepan = video1.mkv) akan diberikan kepada sebuah thread untuk digabung. Penggabungan menggunakan fungsi **gabungVideo** yang implementasinya akan dijelaskan dibawah.

Dibawah ini merupakan fungsi **isVideo** yang digunakan untuk memeriksa apakah sebuah file merupakan pecahan video. Intinya fungsi ini memeriksa apakah namafile yang diperiksa memiliki ekstensi **.000**.
```c
int isVideo(const char *path){
	char temp[1000];
	sprintf(temp, "%s", path);
	int len = strlen(temp);
	char *ext;
	ext = &temp[len - 4];
	if (strcmp(ext, ".000") == 0){
		return 1;
	}
	return 0;
}
```

Kemudian terdapat fungsi **gabungVideo**. Fungsi ini merupakan fungsi yang akan diberikan kepada sebuah thread untuk menggabungkan pecahan pecahan video yang sudah dinyatakan valid. Fungsi **gabungVideo** mengambil sebuah parameter yaitu namadepan dari sebuah file pecahan.
```c
void *gabungVideo(void *param){
	DIR *dp;
	struct dirent *de;
	char *namadepan = (char *) param;
	dp = opendir(dirpath);
	char newVideo[1000];
	char encVideo[1000];
	sprintf(encVideo, "%s", namadepan);
	enc(encVideo);
	sprintf(newVideo, "%s/%s", videosPath, encVideo);
	creat(newVideo, 0777);
	printf("gabung video %s\n", newVideo);
	while((de = readdir(dp)) != NULL){
		char temp[1000];
		sprintf(temp, "%s", de->d_name);
		dec(temp);
		printf("part file %s compare %s \n", temp, namadepan);
		if (strstr(temp, namadepan)){
			FILE *fp = fopen(newVideo, "a+");
			printf("part file status ok");
			char namafile[1000];
			sprintf(namafile, "%s/%s", dirpath, de->d_name);
			printf(" opening %s file and begin to copying ", namafile);
			FILE *fp2 = fopen(namafile, "r");
			
			size_t n, m;
			unsigned char buff[8192];
			do {
					n = fread(buff, 1, sizeof buff, fp2);
					if (n) m = fwrite(buff, 1, n, fp);
					else   m = 0;
			} while ((n > 0) && (n == m));
			
			fclose(fp2);
			fclose(fp);
			printf("\nsucces copying %s\n", temp);
		}
	}
	printf("gabung video selesai\n");
}
```
Terdapat beberapa bagian penting pada fungsi **gabungVideo** diatas.
1.  `while((de = readdir(dp)) != NULL)` untuk menelusuri seluruh file dalam directory root filesystem.
2. `strstr(temp, namadepan)` digunakan untuk memeriksa apakah sebuah directory entry merupakan bagian pecahan lain dari sebuah video yang akan dibuat. temp dan namadepan dicek dengan format yang telah di dekripsi.
3. ```c
    unsigned char buff[8192];
    do {
        n = fread(buff, 1, sizeof buff, fp2);
        if (n) m = fwrite(buff, 1, n, fp);
        else   m = 0;
    } while ((n > 0) && (n == m));

    fclose(fp2);
    fclose(fp);
    ```
    bagian ini digunakan untuk memindahkan seluruh data dari pecahan video yang ditemukan, kemudian me-appendnya dengan file asli yang sedang dibuat. **fp** adalah file asli yang sedang dibuat, **fp2** merupakan file pecahan video yang sedang dideteksi.

**_Hasil_**  
 

## Soal-3
Sebelum diterapkannya file system ini, Atta pernah diserang oleh hacker LAPTOP_RUSAK yang menanamkan user bernama “chipset” dan “ic_controller” serta group “rusak” yang tidak bisa dihapus. Karena paranoid, Atta menerapkan aturan pada file system ini untuk menghapus “file bahaya” yang memiliki spesifikasi:
- Owner Name    : ‘chipset’ atau ‘ic_controller’
- Group Name    : ‘rusak’
- Tidak dapat dibaca  

Jika ditemukan file dengan spesifikasi tersebut ketika membuka direktori, Atta akan menyimpan nama file, group ID, owner ID, dan waktu terakhir diakses dalam file “filemiris.txt” (format waktu bebas, namun harus memiliki jam menit detik dan tanggal) lalu menghapus “file bahaya” tersebut untuk mencegah serangan lanjutan dari LAPTOP_RUSAK.
 
 **_Jawaban_**
 
Fungsi **eksekusiBahaya** digunakan untuk memeriksa apakah suatu file berbahaya atau tidak dan sekaligus menghapus dan menulis lognya apabila dinyatakan berbahaya.  
Fungsi **eksekusiBahaya** akan selalu dipanggil saat melakukan readdir suatu directory entry.
```c
void eksekusiBahaya(const char *path, const char* name){
	struct stat stat;
	lstat(path, &stat);
	struct passwd *pwd = getpwuid(stat.st_uid);
	struct group *grp = getgrgid(stat.st_gid);
	int ans = 0;
	if (strcmp(pwd->pw_name, "ic-controller") == 0 || strcmp(pwd->pw_name, "chipset") == 0){
		if (strcmp(grp->gr_name, "rusak") == 0){
			if (access(path, R_OK) != 0){
				ans = 1;
			}
		}
	}
	printf("eksekusi berbahaya %s %s %s %d\n", path, pwd->pw_name, grp->gr_name, ans);
	if (ans == 1){
		FILE *fp;
		struct tm *tm;
		char buf[200];
		time_t t = stat.st_atime;
		tm = localtime(&t);
		strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", tm);
		char filemirispath[1000];
		sprintf(filemirispath, "%s/%s", dirpath, filemiris);
		printf("filemirispath %s\n", filemirispath);
		fp = fopen(filemirispath, "a");
		fprintf(fp, "%s %s %s %s\n", name, grp->gr_name, pwd->pw_name, buf);
		fclose(fp);
		remove(path);
	}
}
```
Hal penting yang terdapat pada fungsi **eksekusiBahaya** yaitu:
1. ```c
    struct stat stat;
	lstat(path, &stat);
	struct passwd *pwd = getpwuid(stat.st_uid);
	struct group *grp = getgrgid(stat.st_gid);
   ```
   Potongan kode diatas digunakan untuk mengambil stat dari sebuah file. Stat nantinya akan mengandung uid dan gid yang dapat digunakan untuk mencari nama user dan group file tersebut.
2. ```c
    if (strcmp(pwd->pw_name, "ic-controller") == 0 || strcmp(pwd->pw_name,  "chipset") == 0){
      if (strcmp(grp->gr_name, "rusak") == 0){
        if (access(path, R_OK) != 0){
          ans = 1;
        }
      }
    }
   ```
   Potongan kode diatas digunakan untuk memeriksa apakah suatu file berbahaya atau tidak. Tingkat pertama memeriksa apakah user bernama **ic-controller** atau **chipset**. Tingkat kedua memeriksa apakah file tersebut berada pada group **rusak**. Tingkat ketiga memeriksa apakah file tersebut bisa dibaca atau tidak. Apabila ketiga hal tersebut terpenuhi maka akan ditandai.
3. ```c
    fp = fopen(filemirispath, "a");
		fprintf(fp, "%s %s %s %s\n", name, grp->gr_name, pwd->pw_name, buf);
		fclose(fp);
		remove(path);
   ```
   Potongan program tersebut digunakan untuk mengeksekusi file berbahaya. Pertama melakukan log terlebih dahulu kemudian menghapus file tersebut.
  
_**Hasil:**_


## Soal-4
Pada folder **YOUTUBER**, setiap membuat folder permission foldernya akan otomatis menjadi 750. Juga ketika membuat file permissionnya akan otomatis menjadi 640 dan ekstensi filenya akan bertambah “.iz1”. File berekstensi “.iz1” tidak bisa diubah permissionnya dan memunculkan error bertuliskan “File ekstensi iz1 tidak boleh diubah permissionnya.”

**_Jawaban_**


**_Hasil :_**
  
## Soal-5
Ketika mengedit suatu file dan melakukan save, maka akan terbuat folder baru bernama **Backup** kemudian hasil dari save tersebut akan disimpan pada backup dengan nama **namafile_[timestamp].ekstensi**. Dan ketika file asli dihapus, maka akan dibuat folder bernama **RecycleBin**, kemudian file yang dihapus beserta semua backup dari file yang dihapus tersebut (jika ada) di zip dengan nama **namafile_deleted_[timestamp].zip** dan ditaruh ke dalam folder RecycleBin (file asli dan backup terhapus). Dengan format **[timestamp]** adalah **yyyy-MM-dd_HH:mm:ss**

**_Jawaban_**

**_Hasil_**  
