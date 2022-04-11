import argparse
from PIL import Image
import math

argParser = argparse.ArgumentParser(description='convert a png file to mif format')
argParser.add_argument('inputPath', metavar='InPath', type=str, help='the path to input png file')
argParser.add_argument('outputPath', metavar='OutPath', type=str, help='the path to output mif file')
argParser.add_argument('--bw', help='output only binary values for black and white', action='store_true')
args = argParser.parse_args()
inputPath = args.inputPath
outputPath = args.outputPath

"""
if args.bw:
    im = Image.open(inputPath)
    pix = im.load()
    hexDigits = len(hex(math.ceil(im.size[0] * im.size[1] / 8) - 1)) - 2

    outFile = open(outputPath, 'w')
    outFile.truncate(0)
    outFile.write('DEPTH = ' + str(math.ceil(im.size[0] * im.size[1] / 8)) + ';\n')
    outFile.write('WIDTH = 8;\nADDRESS_RADIX = HEX;\nDATA_RADIX = BIN;\nCONTENT\nBEGIN\n')
    count = 0
    index = 0
    for x in range(im.size[1]):
        for y in range(im.size[0]):
            if count == 0:
                outFile.write(hex(index)[2:].zfill(hexDigits) + ' : ')
                index += 1
            if pix[y,x][0] == 0:
                outFile.write('1')
            else:
                outFile.write('0')
            count += 1
            if (count == 8):
                count = 0
                outFile.write(';\n')
    if count != 0:
        while count < 8:
            outFile.write('0')
            count += 1
    outFile.write(';\n')
    outFile.write('END;')
else:
"""

im = Image.open(inputPath)
pix = im.load()
hexDigits = len(hex(im.size[0] * im.size[1] - 1)) - 2

outFile = open(outputPath, 'w')
outFile.truncate(0)
outFile.write('DEPTH = ' + str(im.size[0] * im.size[1]) + ';\n')
if args.bw:
    outFile.write('WIDTH = 8;\nADDRESS_RADIX = HEX;\nDATA_RADIX = HEX;\nCONTENT\nBEGIN\n')
else:
    outFile.write('WIDTH = 24;\nADDRESS_RADIX = HEX;\nDATA_RADIX = HEX;\nCONTENT\nBEGIN\n')
for x in range(im.size[1]):
    for y in range(im.size[0]):
        index = hex(x * im.size[0] + y)[2:].zfill(hexDigits)
        if args.bw:
            '''
            if (pix[y,x][0] == 0):
                outFile.write(index + ' : ff;\n')
            else:
                outFile.write(index + ' : 00;\n')
            '''
            intensity = 255 - pix[y,x][0]
            outFile.write(index + ' : ' + hex(intensity)[2:].zfill(2) + ';\n')
        else:
            r = hex(pix[y,x][0])[2:].zfill(2)
            g = hex(pix[y,x][1])[2:].zfill(2)
            b = hex(pix[y,x][2])[2:].zfill(2)
            outFile.write(index + ' : ' + r + g + b + ';\n')
outFile.write('END;')