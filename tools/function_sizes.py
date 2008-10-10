import operator

mapfile = "C:\\Documents and Settings\\marc\\My Documents\\atom\\Release (Small)\\atom.map"

lines = open(mapfile,'r').readlines()

i = 0
while True:
    if lines[i].startswith("  Address"):
        break
    i = i + 1

i = i + 2

addresses = lines[i:]

functions = [];
last = 0
for a in addresses:
    if len(a) > 5 and a[5] == ':':
        address = a[6:14]
        address = int(address, 16)
        function = a[21:-1]
        size = address - last
        functions.append( (size, function) )

functions = sorted(functions, key=operator.itemgetter(0), reverse=True)

for function in functions:
    print function
