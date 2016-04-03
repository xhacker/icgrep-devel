from subprocess import call, Popen
from collections import *
import csv
import StringIO
import os.path
import sys
import shutil

InputRow = namedtuple('InputRow', ['file','expression','program','stat','value','value2','value3','value4'])

incsv = []
try:
	for row in map(InputRow._make, csv.reader(open("avxoutput.csv", "rb"))):
		incsv.append(row)
except TypeError:
	readFile = open("avxoutput.csv")
	lines = readFile.readlines()
	readFile.close()
	w = open("avxoutput.csv",'w')
	w.writelines([item for item in lines[:-1]])
	w.close()
	for row in map(InputRow._make, csv.reader(open("avxoutput.csv", "rb"))):
		incsv.append(row)


Datfile = namedtuple('Datfile', ['file','program1','program2','stat1','stat2','perbyte','rows'])
Datrow = namedtuple('Datrow', ['number','fileexpressions'])
FileExpression=namedtuple('FileExpression', ['file','expression','filesize'])

charts = []
file1='howto'
exp1='at'
size1=39422105
file2='howto'
exp2='Date'
size2=39422105
file3='howto'
exp3='Email'
size3=39422105
file4='howto'
exp4='URIorEmail'
size4=39422105
file5='roads'
exp5='xquote'
size5=11861751

cpms=3200000

chart1=[]
chart1.append(Datfile('../../../docs/Working/re/data/avxcycles1.dat','bitstreams','bitstreams_avx2','cycles:u','cycles:u',True,(Datrow('0',(FileExpression(file1,exp1,size1),)),Datrow('1',(FileExpression(file2,exp2,size2),)),Datrow('2',(FileExpression(file3,exp3,size3),)),Datrow('3',(FileExpression(file4,exp4,size4),)),Datrow('4',(FileExpression(file5,exp5,size5),)),)))
charts.append(chart1)

chart2=[]
chart2.append(Datfile('../../../docs/Working/re/data/avxinstructions1.dat','bitstreams','bitstreams_avx2','instructions:u','instructions:u',True,(Datrow('0',(FileExpression(file1,exp1,size1),)),Datrow('1',(FileExpression(file2,exp2,size2),)),Datrow('2',(FileExpression(file3,exp3,size3),)),Datrow('3',(FileExpression(file4,exp4,size4),)),Datrow('4',(FileExpression(file5,exp5,size5),)),)))
charts.append(chart2)

chart3=[]
chart3.append(Datfile('../../../docs/Working/re/data/avxipc1.dat','bitstreams','bitstreams_avx2','instructions:u','cycles:u',True,(Datrow('0',(FileExpression(file1,exp1,size1),)),Datrow('1',(FileExpression(file2,exp2,size2),)),Datrow('2',(FileExpression(file3,exp3,size3),)),Datrow('3',(FileExpression(file4,exp4,size4),)),Datrow('4',(FileExpression(file5,exp5,size5),)),)))
charts.append(chart3)

#chart4=[]
#chart4.append(Datfile('../../../docs/Working/re/data/avxbranch-misses.dat','bitstreams_avx2','bitstreams_avx2','branch-misses:u','',True,(Datrow('0',(FileExpression(file1,exp1,size1),)),Datrow('1',(FileExpression(file2,exp2,size2),)),Datrow('2',(FileExpression(file3,exp3,size3),)),Datrow('3',(FileExpression(file4,exp4,size4),)),Datrow('4',(FileExpression(file5,exp5,size5),)),)))
#chart4.append(Datfile('../../../docs/Working/re/data/ssebranch-misses.dat','bitstreams','bitstreams','branch-misses:u','',True,(Datrow('0',(FileExpression(file1,exp1,size1),)),Datrow('1',(FileExpression(file2,exp2,size2),)),Datrow('2',(FileExpression(file3,exp3,size3),)),Datrow('3',(FileExpression(file4,exp4,size4),)),Datrow('4',(FileExpression(file5,exp5,size5),)),)))
#charts.append(chart4)

chart5=[]
chart5.append(Datfile('../../../docs/Working/re/data/avxtime.dat','bitstreams_avx2','bitstreams_avx2','cycles:u','',True,(Datrow('0',(FileExpression(file1,exp1,size1*cpms),)),Datrow('1',(FileExpression(file2,exp2,size2*cpms),)),Datrow('2',(FileExpression(file3,exp3,size3*cpms),)),Datrow('3',(FileExpression(file4,exp4,size4*cpms),)),Datrow('4',(FileExpression(file5,exp5,size5*cpms),)),)))
chart5.append(Datfile('../../../docs/Working/re/data/ssetime.dat','bitstreams','bitstreams','cycles:u','',True,(Datrow('0',(FileExpression(file1,exp1,size1*cpms),)),Datrow('1',(FileExpression(file2,exp2,size2*cpms),)),Datrow('2',(FileExpression(file3,exp3,size3*cpms),)),Datrow('3',(FileExpression(file4,exp4,size4*cpms),)),Datrow('4',(FileExpression(file5,exp5,size5*cpms),)),)))
charts.append(chart5)



for chart in charts:
	chartmin=10000000
	for datfile in chart:
		for datrow in datfile.rows:
			value1=0
			valuecount1=0
			value2=0
			valuecount2=0
			for row in incsv:
				for fileexpression in datrow.fileexpressions:
					if row.expression==fileexpression.expression and row.file == fileexpression.file and row.program==datfile.program1 and row.stat==datfile.stat1:
						if datfile.perbyte:
							value1=value1+(float(row.value)/fileexpression.filesize)
						else:
							value1=value1+int(row.value)
						valuecount1=valuecount1+1
					if row.expression==fileexpression.expression and row.file == fileexpression.file and row.program==datfile.program2 and row.stat==datfile.stat2:
						if datfile.perbyte:
							value2=value2+(float(row.value)/fileexpression.filesize)
						else:
							value2=value2+int(row.value)						
						valuecount2=valuecount2+1
			print value1
			print value2
			if valuecount1>0:
				value1 = value1/valuecount1
			if valuecount2>0:
				value2 = value2/valuecount2
			if value2>0:
				value = value1/value2
			else:
				value = value1
			if value<chartmin:
				chartmin=value
	for datfile in chart:
		f = open(datfile.file, 'w')
		for datrow in datfile.rows:
			value1=0
			valuecount1=0
			value2=0
			valuecount2=0
			for row in incsv:
				for fileexpression in datrow.fileexpressions:
					if row.expression==fileexpression.expression and row.file == fileexpression.file and row.program==datfile.program1 and row.stat==datfile.stat1:
						if datfile.perbyte:
							value1=value1+(float(row.value)/fileexpression.filesize)
						else:
							value1=value1+int(row.value)
						valuecount1=valuecount1+1
					if row.expression==fileexpression.expression and row.file == fileexpression.file and row.program==datfile.program2 and row.stat==datfile.stat2:
						if datfile.perbyte:
							value2=value2+(float(row.value)/fileexpression.filesize)
						else:
							value2=value2+int(row.value)						
						valuecount2=valuecount2+1
			print value1
			print value2
			if valuecount1>0:
				value1 = value1/valuecount1
			if valuecount2>0:
				value2 = value2/valuecount2
			if value2>0:
				value = value1/value2
			else:
				value = value1
			if False:#value>chartmin*10:
				value=chartmin*10
			f.write(datrow.number)
			f.write(' ')
			f.write(str(value))
			f.write('\n')
		f.close()
