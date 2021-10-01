#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "process.h"
#include "threads/vaddr.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/malloc.h"
#include "devices/input.h"

static void syscall_handler (struct intr_frame *);
void handle(int syscallnum, struct intr_frame*);
void user_halt(void);
int user_exec(const char* cmd_line);

// project 2
int user_write(int fd, const void* buffer, unsigned size);
int user_read(int fd, void* buffer, unsigned size);
int user_wait(tid_t tid);
bool user_create(const char* file, unsigned initial_size);
bool user_remove(const char* file);
int user_open(const char* name_file);
void user_close(int fd);
int user_filesize(int fd);
void user_seek(int fd, unsigned position);
unsigned int user_tell(int fd);

// Project 2
int temp_save_tid[40];
int name_flag, lock_flag;
int prev_fd;
char file_name[128][40];
struct lock syscall_lock;
// ##############################

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	// Project 2
	int* intr_esp = f->esp;
	int syscallnum = 0;

	syscallnum = *intr_esp;
	if (!lock_flag) {
		lock_init(&syscall_lock);
		lock_flag = 1;
	}

	handle(syscallnum, f);
	// ##############################

}

// Project 2
void handle(int syscallnum, struct intr_frame* f UNUSED) {
	int* intr_esp = f->esp;

	if ((f->esp) >= PHYS_BASE - 10)
		return user_exit(-1);

	if (syscallnum == SYS_HALT)
		user_halt();
	else if (syscallnum == SYS_EXEC)
		f->eax = user_exec((const char*)intr_esp[1]);
	else if (syscallnum == SYS_WRITE)
		f->eax = user_write(intr_esp[1], (const void*)intr_esp[2], intr_esp[3]);
	else if (syscallnum == SYS_READ)
		f->eax = user_read(intr_esp[1], (void*)intr_esp[2], intr_esp[3]);
	else if (syscallnum == SYS_WAIT)
		f->eax = user_wait(intr_esp[1]);
	else if (syscallnum == SYS_EXIT)
		user_exit(intr_esp[1]);
	
	else if (syscallnum == SYS_CREATE) {
		f->eax = user_create((const char*)intr_esp[1], intr_esp[2]);
	}
	else if (syscallnum == SYS_REMOVE) {
		f->eax = user_remove((const char*)intr_esp[1]);
	}
	else if (syscallnum == SYS_OPEN) {
		f->eax = user_open((const char*)intr_esp[1]);
	}
	else if (syscallnum == SYS_CLOSE) {
		user_close(intr_esp[1]);
	}
	else if (syscallnum == SYS_FILESIZE) {
		f->eax = user_filesize(intr_esp[1]);
	}
	else if (syscallnum == SYS_SEEK) {
		user_seek(intr_esp[1], intr_esp[2]);
	}
	else if (syscallnum == SYS_TELL) {
		f->eax = user_tell(intr_esp[1]);
	}
	else
		return;
}
// ##############################

unsigned int user_tell(int fd) {
	struct thread* cur = thread_current();
	off_t tell_check;

	tell_check = file_tell(cur->file_list[fd]);		// ���� ����Ʈ ������� file_tell

	return tell_check;
}
// ##############################

void user_seek(int fd, unsigned position) {
	struct thread* cur = thread_current();

	if (cur->file_list[fd] != NULL) {
		file_seek(cur->file_list[fd], position); // file_list�� file�� ������, file_seek
	}
}
// ##############################

int user_filesize(int fd) {
	struct thread* cur;
	off_t file_len;

	cur = thread_current();

	if (cur->file_list[fd] != NULL) {
		file_len = file_length(cur->file_list[fd]);
		return file_len;
	}
	else
		return 0;
}


int user_write(int fd, const void* buffer, unsigned size)
{
	struct thread* cur = thread_current();
	int byte_size = 0;

	if (fd == 1) {	//fd�� console
		lock_acquire(&syscall_lock);	// lock�� acquire
		putbuf(buffer, size);  // putbuf
		lock_release(&syscall_lock);  // unlock
		return size;
	}

	else if (fd == 0 || fd < 0) {
		return -1;
	}

	else {
		if (cur->file_list[fd] != NULL) {		// ���� thread�� file_list�� file�� �����Ҷ�
			if (!file_deny_check(cur->file_list[fd])) {		// file_deny_check
				lock_acquire(&syscall_lock);		//lock
				byte_size = file_write(cur->file_list[fd], buffer, size);	// true�̸� true, false�̸� false return
				lock_release(&syscall_lock);	// unlock
			}
		}

		else {
			return -1;
		}
	}
	return byte_size;
}

// ##############################

void user_close(int fd) {
	struct thread* cur;

	cur = thread_current();

	if (cur->file_list[fd] != NULL) {  // file_list�� ������
		file_close(cur->file_list[fd]);	 // file_close
		cur->fd--;
		cur->file_list[fd] = NULL; // list�� file NULL��
	}
}
// ##############################

bool user_create(const char* file, unsigned initial_size) {
	bool create_check;   // ������ �����Ǿ����� check

	if (file == NULL) {
		user_exit(-1);    // file�� NULL�϶� user_exit(-1)
	}

	create_check = filesys_create(file, initial_size); // filesys_create
	return create_check;   // create_check return
}
// ##############################

bool user_remove(const char* file) {
	bool remove_check;
	remove_check = filesys_remove(file);		//filesys_remove�� file ����
	return remove_check;
}


int user_open(const char* name_file) {
	struct file* opening_file = NULL;  // file_open�� �Ǿ�����, open�� ���� ����
	struct thread* cur;  // current thread
	int file_cnt;  // ���� file_list�� index

	if (name_file == NULL) {
		return -1;	// filename�� ���� ��� -1 return
	}

	cur = thread_current();

	opening_file = filesys_open(name_file);  // filesys_open�� �� opening_file�� ����

	if (cur->fd == 0) {   // ���� fd�� 0�̰�, 
		if (prev_fd == 0) // open file������
			file_cnt = 2;  // 2�� �ʱ�ȭ
		else
			file_cnt = prev_fd + 1;  // open�� file ������ 0�� �ƴϸ� 1����
	}

	else {
		file_cnt = cur->fd;
		file_cnt++;
	}

	if (opening_file == NULL) {		// file_open�ȵǸ� -1 return
		return -1;
	}

	else {		//file_open �Ǹ� filelist�� ���� ������ ���� ����
		strlcpy(file_name[file_cnt], name_file, 40);
		cur->file_list[file_cnt] = opening_file;

		if (find_executable_file(name_file))
			file_deny_write(opening_file);
		// chlid thread�� parent thread�� ���� ���� ���ϰ� deny_write
		cur->fd = file_cnt;
		prev_fd = cur->fd;

		return cur->fd;
	}
}

int user_wait(tid_t tid) {
	return process_wait(tid);
}

void user_halt(void) {
	shutdown_power_off();
}

int user_exec(const char* cmd_line) {
	tid_t execute_res;

	execute_res = process_execute(cmd_line);

	return execute_res;
}



int user_read(int fd, void* buffer, unsigned size)
{
	struct thread* cur = thread_current();

	int count = 0;

	if (!fd) { // fd�� 0�̸�
		lock_acquire(&syscall_lock);
		count = input_getc();  //input_getc 
		lock_release(&syscall_lock);  // lock �ɰ� Ǯ��
	}

	else if (fd == 1 || fd < 0) {
		return -1;
	}

	else if (buffer >= PHYS_BASE - 10) {
		user_exit(-1);
	}

	else {
		if (cur->file_list[fd] != NULL) {	// ���� list�� NULL�� �ƴϸ� 
			lock_acquire(&syscall_lock);	// lockd�� acquire
			count = file_read(cur->file_list[fd], buffer, size);	// file_read ȣ���Ͽ� ���Ͽ� �����Ͽ� read
			lock_release(&syscall_lock); // read������ lock�� Ǯ�� �ٸ� ���μ����� ���ٰ���
		}

		else {
			return -1;
		}
	}

	return count;
}

void user_exit(int status)
{
	struct thread* thread_now;
	char* save_ptr, * exit_name, tmp[20];

	thread_now = thread_current();
	strlcpy(tmp, thread_now->name, 20);
	exit_name = strtok_r(tmp, " ", &save_ptr);
	thread_now->exitstate = status;		// ���� thread�� parameter�� ���� status ����

	if (thread_now->parent->status == THREAD_BLOCKED)
		printf("%s: exit(%d)\n", exit_name, status);		// ���� thread�� parent�� status�� block�̸� exit �޼���
	while (!list_empty(&thread_now->parent->wait_sema.waiters)) {	// ���� thread�� parent�� wait_sema �� �����ϰ�
		if (thread_now->tid == thread_now->parent->wait_child_tid)	// ���� thread�� tid�� ���� thread�� parent�� ��ٸ���
			// child�� ���ٸ�
			sema_up(&thread_now->wait_sema);		// ���� thread�� ���� sema_up
	}

	sema_up(&thread_now->wait_sema);

	thread_exit();
}
