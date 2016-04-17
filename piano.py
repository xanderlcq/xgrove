lines = tuple(open('/Users/Xander/Desktop/piano/raw_data', 'r'))
arr = []
for line in lines:
    arr.append(line.rstrip())
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
print 'each row in split binary: '+str(arr_binary_split)
arr_dec= []
for row in arr_binary_split:
    row_dec = []
    for i in row:
        row_dec.append(int(str(i),2))
    arr_dec.append(row_dec)
print 'each row decimal: '+ str(arr_dec)

row_sum = []
for row in arr_dec:
    a = 0
    for i in row:
        a+=i
    row_sum.append(a)

print 'each row decimal sum: '+str(row_sum)
#print sorted(row_sum)
# 11{ 14 15t s21 23 25 26} 32 39f 41 45 47 49 53c 54 77
arr_temp = []
for i in row_sum:
    if i == 11:
        arr_temp.append('{')
    elif i == 15:
        arr_temp.append('t')
    elif i == 21:
        arr_temp.append('s')
    elif i == 26:
        arr_temp.append('}')
    elif i == 39:
        arr_temp.append('f')
    elif i == 53:
        arr_temp.append('c')
    else:
        arr_temp.append(str(i))
#print arr_temp
print '=============================='

lines = tuple(open('/Users/Xander/Desktop/piano/1_7', 'r'))
arr = []
for line in lines:
    arr.append(line.rstrip())
arr_binary_split = []
arr_str = []
arr_nocomma = []
for line in arr:
    row = line.split(',')

    r = ''
    for i in row:
        if i!='':
            i = str(int(i))
        r+=i
    arr_nocomma.append(r)
    arr_str.append(row)
    int_row = []
    for r in row:

        if r!='':
            int_row.append(int(r))
    arr_binary_split.append(int_row)
print 'each row\'s string split'+str(arr_str)
sins = []

print 'each row without comma'+str(arr_nocomma)
print 'each row without comma, sorted'+str(sorted(arr_nocomma))
arr_dec= []
for row in arr_binary_split:
    row_dec = []
    for i in row:
        row_dec.append(int(str(i)))
    arr_dec.append(row_dec)
print 'each row decimal: '+ str(arr_dec)

row_sum = []
for row in arr_dec:
    a = 0
    for i in row:
        a+=i
    row_sum.append(a%7)

print 'each row decimal sum: '+str(row_sum)
print sorted(row_sum)
guess = []
for i in row_sum:
    guess.append(str(chr(i*7)))
print guess
s = ''
for i in guess:
    s+=i
print s
# 444s 475c 442t 472f 432{ 436}