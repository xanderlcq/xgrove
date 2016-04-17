lines = tuple(open('/Users/Xander/Desktop/piano/morse', 'r'))
arr = []
for line in lines:
    arr.append(line.rstrip())
print arr
arr_binary_split = []
arr_str = []
arr_nocomma = []
for line in arr:
    row = line.split(',')
    r = ''
    for i in row:
        r+=i
    arr_nocomma.append(r)
    arr_str.append(row)
    int_row = []
    for r in row:

        if r!='':
            int_row.append(int(r))
    arr_binary_split.append(int_row)
print 'each row\'s string split'+str(arr_str)
print 'each row without comma'+str(arr_nocomma)
base_s = []
for i in arr_nocomma:
    base_s.append(int(i,7))
print base_s
answer = []
for i in base_s:
    answer.append(chr(i))
s = ''
for k in answer:
    s+=k
print s
