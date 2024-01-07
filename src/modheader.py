import numpy as np
import argparse

def main():
    parser = argparse.ArgumentParser(
                    prog='MCH',
                    description='Place the correct checksum on the parser')
    parser.add_argument('filename')
    args = parser.parse_args()

    with open(args.filename, mode='rb') as f:
        data = bytearray(f.read())
        f.close()

    offset = 5
    nrbytes = np.uint16(len(data) - offset)
    checksum = np.sum(data[offset:], dtype=np.uint16)

    print('Length:   0x%04X' % nrbytes)
    print('Checksum: 0x%04X' % checksum)

    # remember that Z80 is little endian (LSB first)
    data[0x01] = nrbytes & 0xFF
    data[0x02] = (nrbytes >> 8) & 0xFF
    data[0x03] = (~(checksum & 0xFF) + 1) & 0xFF
    data[0x04] = ~((checksum >> 8) & 0xFF) & 0xFF

    # update main.rom
    with open(args.filename, mode='wb') as f:
        f.write(data)
        f.close()

if __name__ == '__main__':
    main()