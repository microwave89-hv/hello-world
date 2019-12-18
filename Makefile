OBJS	= main.o
TARGET	= hello_world.efi
SRC	= main.c

CC=/usr/local/opt/llvm/bin/clang
CFLAGS+= \
	--target=x86_64-unknown-windows \
	-ffreestanding \
	-fshort-wchar \
	-flto \
	-mno-red-zone \
	-pedantic \
	-O2 \
	-Wall \

LDFLAGS+= \
	--target=x86_64-unknown-windows \
	-nostdlib \
	-Wl,-entry:efi_miau \
	-Wl,-subsystem:efi_application \
	-fuse-ld=lld-link

all: $(TARGET)

hello_world.efi: main.o
	$(CC) -o hello_world.efi $(LDFLAGS) main.o

main.o:
	$(CC) -c -o main.o $(CFLAGS) main.c

.PHONY: clean install

clean:
	rm ./*.o ./*.efi ./*.S ./*.s

MNTDIR = /Volumes/EFIMOUNT/
TARGETDIR = /Volumes/EFIMOUNT/EFI/MyApps/
MNTDEV = /dev/disk0s1
install:
	sudo mkdir -p $(MNTDIR)
	sudo diskutil mount -mountPoint $(MNTDIR) $(MNTDEV)
	sudo mkdir -p $(TARGETDIR)
	cp hello_world.efi $(TARGETDIR)
	sync
	sudo diskutil unmount $(MNTDEV)
