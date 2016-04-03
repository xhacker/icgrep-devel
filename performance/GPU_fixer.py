import sys, re, os, shutil

backup_extension = '.bak'

hasbit_RE = re.compile(r'if \(\(bitblock::any[(](.*)\) \|\|')

def fix1_hasbit_in_source(matchobj):
        return "if ((bitblock_has_bit(pablo_or(idx,%s,carry)) ||" % (matchobj.group(1))

def hasbit_fixer(s):
        s = re.sub(hasbit_RE, fix1_hasbit_in_source, s)
        return s

Advance_RE = re.compile(r'carryQ.cq\[([0-9]*)\] = carryout2carry\(pablo_blk_Advance\((.*),\s*carryQ.get_carry_in\([0-9]*\), ([^)]*)\)\);')
#Advance_RE = re.compile(r'Carry([0-9]*) = bitblock::srli<127>\(pablo_blk_Advance\((.*),\s*Carry[0-9]*, ([^)]*)\)\);')

def fix1_Advance_in_source(matchobj):
        return "%s = pablo_advance(idx, %s, carry, group_carry, %s);" % (matchobj.group(3), matchobj.group(2), matchobj.group(1))

def Advance_fixer(s):
        s = re.sub(Advance_RE, fix1_Advance_in_source, s)
        return s

MatchStar_RE = re.compile(r'carryQ.cq\[([0-9]*)\] = carryout2carry\(pablo_blk_MatchStar\((.*),\s*carryQ.get_carry_in\([0-9]*\), ([^)]*)\)\);')
#MatchStar_RE = re.compile(r'Carry([0-9]*) = bitblock::srli<127>\(pablo_blk_MatchStar\((.*),\s*Carry[0-9]*, ([^)]*)\)\);')

def fix1_MatchStar_in_source(matchobj):
        return "%s = pablo_blk_MatchStar(idx, %s, carry, bubble, group_carry, %s);" % (matchobj.group(3), matchobj.group(2), matchobj.group(1))

def MatchStar_fixer(s):
        s = re.sub(MatchStar_RE, fix1_MatchStar_in_source, s)
        return s

simd_const_RE = re.compile(r'simd<([0-9]*)>::constant<([0-9]*)>\(\)')

def fix1_const_in_source(matchobj):
        return "simd_const_%s(%s)" % (matchobj.group(1), matchobj.group(2))

def simd_const_fixer(s):
        s = re.sub(simd_const_RE, fix1_const_in_source, s)
        return s
 
CarryTest_RE = re.compile(r'carryQ.CarryTest\(')
#Advance_RE = re.compile(r'Carry([0-9]*) = bitblock::srli<127>\(pablo_blk_Advance\((.*),\s*Carry[0-9]*, ([^)]*)\)\);')

def fix1_CarryTest_in_source(matchobj):
        return "CarryTest(group_carry, "

def CarryTest_fixer(s):
        s = re.sub(CarryTest_RE, fix1_CarryTest_in_source, s)
        return s

CarryEnqueue_RE = re.compile(r'carryQ.CarryDequeueEnqueue[^;]*;')
#Advance_RE = re.compile(r'Carry([0-9]*) = bitblock::srli<127>\(pablo_blk_Advance\((.*),\s*Carry[0-9]*, ([^)]*)\)\);')

def fix1_Enqueue_in_source(matchobj):
        return ""

def CarryEnqueue_fixer(s):
        s = re.sub(CarryEnqueue_RE, fix1_Enqueue_in_source, s)
        return s

CarryAdjust_RE = re.compile(r'carryQ.CarryQ_Adjust[^;]*;')
#Advance_RE = re.compile(r'Carry([0-9]*) = bitblock::srli<127>\(pablo_blk_Advance\((.*),\s*Carry[0-9]*, ([^)]*)\)\);')

def fix1_Adjust_in_source(matchobj):
        return ""

def CarryAdjust_fixer(s):
        s = re.sub(CarryAdjust_RE, fix1_Adjust_in_source, s)
        return s

        
        
def fix_all(s):
        return CarryAdjust_fixer(CarryEnqueue_fixer(CarryTest_fixer(simd_const_fixer(MatchStar_fixer(Advance_fixer(hasbit_fixer(s)))))))

def fix_file(path):
        infile = open(path)
        indata = infile.read()
        infile.close()
        outdata = fix_all(indata)
        if outdata != indata:
                print("Modifying %s" % (path))
                # if not os.path.exists(path + backup_extension): shutil.move(path, path + backup_extension)
                shutil.move(path, path + backup_extension)
                outfile = open(path, 'w')
                outfile.write(outdata)
                outfile.close()

if __name__ == '__main__':
        fix_path = sys.argv[1]
        fix_file(fix_path)

