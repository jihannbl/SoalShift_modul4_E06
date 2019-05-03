#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <sys/xattr.h>
#include <sys/wait.h>
#include <pthread.h>

static const char *dirpath = "/home/kulguy/shift4";
char youtuber[1000] = "/YOUTUBER";
char iz1[1000] = ".iz1";
char filemiris[1000] = "filemiris.txt";
char videosPath[1000];
pthread_t buruhVideo[1000];
int counter;

char list[100] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";

void enc(char* encrypt)
{
	if(!strcmp(encrypt,".") || !strcmp(encrypt,"..")) return;
	int enkripsi = strlen(encrypt);
	for(int i=0;i<enkripsi;i++)
	{
		for(int j=0;j<94;j++)
		{
			if(encrypt[i]==list[j])
			{
				encrypt[i] = list[(j+17)%94];
				break;
			}
		}
	}
}

void dec(char* decrypt)
{
	if(!strcmp(decrypt,".") || !strcmp(decrypt,"..")) return;
	int dekripsi = strlen(decrypt);
	for(int i=0;i<dekripsi;i++)
	{
		for(int j=0;j<94;j++){
			if(decrypt[i]==list[j]){
				decrypt[i] = list[(j+77)%94];
				break;
			}
		}
	}
}

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

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	char name[1000];
	sprintf(name,"%s",path);
	enc(name);
	sprintf(fpath, "%s%s",dirpath,name);
	printf("getattr custom %s\n", fpath);
	if (strstr(fpath, youtuber)){
		char newNamePath[1000];
		int len = strlen(fpath);
		strcpy(newNamePath, fpath);
		newNamePath[len - 4] = '\0';
		char *ext = &fpath[len - 4];
		if (strcmp(ext, iz1) == 0){
			res = lstat(newNamePath, stbuf);
		}else{
			res = lstat(fpath, stbuf);
		}
	}else{
		res = lstat(fpath, stbuf);
	}
	if (res != 0){
		return -ENOENT;
	}

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi){
	int res;
  DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;
	char fpath[1000];
	char name[1000];
	if (strcmp(path, "/") == 0){
		sprintf(fpath, "%s", dirpath);
	}else{
		sprintf(name,"%s",path);
		enc(name);
		sprintf(fpath, "%s%s",dirpath,name);
	}
	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		char fullpathname[1000];
		sprintf(fullpathname, "%s/%s", fpath, de->d_name);
		char temp[1000];
		strcpy(temp,de->d_name);
		dec(temp);
		printf("path %s fpath %s youtuber %s fpname %s\n", path, fpath, youtuber, fullpathname);
		char getExt[1000];
		sprintf(getExt, "%s", temp);
		char *ext = &getExt[strlen(getExt) - 3];
		int bil = atoi(ext);
		if ((bil > 0 && bil < 1000) || strcmp(ext, "000") == 0){
			continue;
		}else if (strstr(fpath, youtuber) && de->d_type == DT_REG){
			char newPathName[1000];
			sprintf(newPathName, "%s.iz1", temp);
			res = (filler(buf, newPathName, &st, 0));
			eksekusiBahaya(fullpathname, newPathName);
		}else{
			res = (filler(buf, temp, &st, 0));
			eksekusiBahaya(fullpathname, temp);
		}
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi){
  char fpath[1000];
	char name[1000];
	if (strcmp(path, "/") == 0){
		sprintf(fpath, "%s", dirpath);
	}else{
		sprintf(name,"%s",path);
		enc(name);
		sprintf(fpath, "%s%s",dirpath,name);
	}
	int res = 0;
  int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	char name[1000];
	sprintf(name,"%s",path);
	enc(name);
	sprintf(fpath, "%s%s",dirpath,name);
	printf("mkdir %s\n", fpath);
	if (strstr(fpath, youtuber)){
		res = mkdir(fpath, 0750);
	}else{
		res = mkdir(fpath, mode);
	}
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {
	(void) fi;

	int res;
	char fpath[1000];
	char name[1000];
	sprintf(name,"%s",path);
	enc(name);
	sprintf(fpath, "%s%s",dirpath,name);
	printf("create %s\n", fpath);
	if (strstr(fpath, youtuber)){
		res = creat(fpath, 0640);
	}else{
		res = creat(fpath, mode);
	}
	if(res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	char name[1000];
	sprintf(name,"%s",path);
	enc(name);
	sprintf(fpath, "%s%s",dirpath,name);
	if (strstr(fpath, youtuber)){
		pid_t child = fork();
		int status;
		if (child == 0){
			// system("zenity --error --title=\"Pesan error\" --text=\"File ekstensi iz1 tidak boleh diubah permissionnya.\n\"");
			char *argv[5] = {"zenity", "--error", "--title=Pesan error", "--text=File ekstensi iz1 tidak boleh diubah permissionnya.", NULL};
			execv("/usr/bin/zenity", argv);
		}else{
			while (wait(&status) > 0);
		}
	}else{
		res = chmod(fpath, mode);	
	}
	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_truncate(const char *path, off_t size)
{
	int res;
    char fpath[1000];
    char name[1000];
	sprintf(name,"%s",path);
    enc(name);
	sprintf(fpath, "%s%s",dirpath,name);
	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	char fpath[1000], name[1000];
	strcpy(name, path);
	enc(name);

	if(strcmp(name,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,name);

	int res;

	res = utimensat(0, fpath, ts, AT_SYMLINK_NOFOLLOW);
	
	if (res == -1)
		return -errno;

	return 0;
}
static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	char fpath[1000];
	char name[1000];
	sprintf(name,"%s",path);
	enc(name);
	sprintf(fpath, "%s%s",dirpath,name);
	res = lchown(fpath, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

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

void xmp_destroy(void *private_data){
	int i = 0;
	for (i = 0; i < counter; i++){
		pthread_join(buruhVideo[i], NULL);
	}
	DIR *dp = opendir(videosPath);
	struct dirent *de;
	while((de = readdir(dp)) != NULL){
		char namafiledelete[1000];
		if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0){
			sprintf(namafiledelete, "%s", de->d_name);
			char temp[1000];
			sprintf(temp, "%s/%s", videosPath, namafiledelete);
			printf("temp delete %s\n", temp);
			remove(temp);
		}
	}
	printf("temp delete %s\n", videosPath);
	rmdir(videosPath);
}

int posisi(char *s, char ch){
	char *a = NULL;
	char *b = strchr(s, ch);

 	while(b != NULL){
		a = b;
 		b = strchr(b+1, ch);
	}
	if(a==NULL)
		return 0;

 	return (int) (a-s);
 }

static int xmp_unlink(const char *path)
{
	char fpath[1000], temp[1000];
	strcpy(temp, path);
	enc(temp);

	if(strcmp(temp,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,temp);

	int res, status;

	if(access(fpath, F_OK)<0)			
		return 0;

	char soal[1000], soal2[1000], waktu[100], file_zip[1000], fileasli[1000], ext[1000],
			namafile[1000], pathNow[1000];
	
	char fd_recyclebin[100]="/home/jihan/shift4/oO.k.EOX[)";
	mkdir(fd_recyclebin, 0777);

	int posisi_ext = posisi(path, '.');
	
	if (posisi_ext==0)
		posisi_ext = strlen(path);
	else{
		strcpy(ext, path+posisi_ext);
	}
	strcpy(fileasli, path);
	enc(fileasli);	

	int posisi_slash = posisi(path, '/');
	int dot = posisi_ext-(posisi_slash+1);
	strncpy(namafile, path+1+posisi_slash, dot);
	namafile[dot] = '\0';

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(waktu, "%04d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	sprintf(file_zip, "%s_deleted_%s", namafile, waktu);
	char zip_temp[1000], file_zip_temp[1000]; 
	char zip[100]="`S[u";
	enc(file_zip);
	enc(namafile);
	
	sprintf(file_zip_temp, "%s.zip", file_zip);
	sprintf(zip_temp, "%s%s", file_zip, zip);

	char backup[100] = "XB.Jhu"; char recycle[100]= "oO.k.EOX[)";
	sprintf(soal, "cd %s && zip '%s/%s' '%s' '%s/%s'* && rm -f '%s/%s'*", dirpath, recycle, file_zip, fileasli, backup, namafile, backup, namafile);
	sprintf(soal2, "%s && cp '%s/%s' '%s/%s' && rm -f '%s/%s'", soal, recycle, file_zip_temp, recycle, zip_temp, recycle, file_zip_temp); 
	if (fork()==0)
		execlp("bash","bash", "-c", soal2, NULL);

		while((wait(&status))>0);


	res = unlink(fpath);
	
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
    char fpath[1000];
    char name[1000];
    char temp[1000];
	sprintf(name,"%s",path);
    enc(name);
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,name);
	
	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	
	strcpy(temp,path); enc(temp);
	sprintf(name, "%s/%s", dirpath,temp);
	
		if(access(name, R_OK)<0)				
		return res;
	
	char fd_backup[1000]="/home/jihan/shift4/XB.Jhu";
	mkdir(fd_backup, 0777);
	
	char namafile[1000], ext[100], waktu[1000], filejadi[1000];
	
	int posisi_ext = posisi(path,'.');
	
	if (posisi_ext==0) {
		posisi_ext = strlen(path);
		ext[0] = '\0';
	}
	else{
		strcpy(ext, path+posisi_ext);
	}
	int posisi_slash = posisi(path, '/');
	int dot = posisi_ext-(posisi_slash+1);
	strncpy(namafile, path+1+posisi_slash, dot);
	namafile[dot] = '\0';
	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(waktu, "%04d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	sprintf(filejadi, "%s_%s", namafile, waktu); 
	strcat(filejadi, ext);	
	enc(filejadi);
	sprintf(name, "%s%s", dirpath, temp);
	
	char final[1000];
	sprintf(final, "%s/%s", fd_backup, filejadi);

	int status;
	if (fork()==0)
		execlp("cp","cp", name, final, NULL);

	while((wait(&status))>0);
	
	return res;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
  .mkdir = xmp_mkdir,
	.chmod = xmp_chmod,
	.chown = xmp_chown,
	.create = xmp_create,
	.init = xmp_init,
	// .setxattr = xmp_setxattr,
	.truncate = xmp_truncate,
	.utimens = xmp_utimens,
	.destroy = xmp_destroy,
	.unlink = xmp_unlink,
  .write = xmp_write,
	// .mknod = xmp_mknod,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
