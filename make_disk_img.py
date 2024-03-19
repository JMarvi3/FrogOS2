import struct
DISK_SIZE = 16 * 1024 * 2
with open('disk.img', 'wb') as f:
    for i in range(DISK_SIZE):
        f.write(struct.pack("128I", *[i]*128))
