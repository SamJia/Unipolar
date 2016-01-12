import re, Queue, os
import numpy as np
from time import clock
import cProfile

class ILLEGAL_POS(Exception):
	def __str__(self):
		return 'illegal_pos'

class ILLEGAL_HANDICAP(Exception):
	def __str__(self):
		return 'illegal_handicap'
		
class NOT_EMPTY(Exception):
	def __str__(self):
		return 'not_empty'
		
def readInfo(content, abs_name):
	global p, record
	if bd_re.search(content) and wd_re.search(content):
		p += 1
		record = True

def readHandicap(content):
	global record
	result = [];
	handicap_match = handicap_re.search(content)
	if handicap_match:
		record = False
		# return
		handicaps = handipos_re.findall(handicap_match.group(1))
		# print content
		# print handicaps, bd_re.search(content).group(0), wd_re.search(content).group(0)
		for p in handicaps:
			if p[0] not in 'abcdefghijklmnopqrs' or p[1] not in 'abcdefghijklmnopqrs':
				raise ILLEGAL_HANDICAP()
			pos_x = ord(p[0]) - ord('a') + BIAS
			pos_y = ord(p[1]) - ord('a') + BIAS
			result.append((pos_x, pos_y))
	return result

def readPaces(content):
	result = [];
	paces = pace_re.findall(content)
	# print paces
	for c,p in paces:
		if p[0] not in 'abcdefghijklmnopqrs' or p[1] not in 'abcdefghijklmnopqrs':
			# print p
			raise ILLEGAL_POS()
			# exit(0)
		color = {'B':BLACK_POINT, 'W':WHITE_POINT}[c]
		pos_x = ord(p[0]) - ord('a') + BIAS
		pos_y = ord(p[1]) - ord('a') + BIAS
		result.append((color, pos_x, pos_y))
	return result

def write(fp, c, matrix):
	matrix = matrix.flatten()
	medium = SCOPE * SCOPE / 2
	tmp_tuple = tuple(matrix[:medium]) + tuple(matrix[medium+1:])
	fp.write('%d ' % c)
	fp.write(FORMAT % tmp_tuple)
edge = 0
empty = 0
save = 0
def saveState(fp, pace):
	global edge, empty, save
	half_scope = SCOPE / 2
	c = pace[0]
	x = pace[1]
	y = pace[2]
	tmp = board[x-half_scope:x+(half_scope+1), y-half_scope:y+(half_scope+1)]
	if EDGE_POINT in tmp:
		edge += 1
		return
	if (tmp != 0).sum() < 3:
		empty += 1
		return
	save += 1
	black = (tmp == BLACK_POINT)
	white = (tmp == WHITE_POINT)
	# fp.write('%d ' % c)
	# result = np.zeros(8)
	# result[0] = black[MASK1].sum()
	# result[1] = black[MASK2].sum()
	# result[2] = black[MASK3].sum()
	# result[3] = black[MASK4].sum()
	# result[4] = white[MASK1].sum()
	# result[5] = white[MASK2].sum()
	# result[6] = white[MASK3].sum()
	# result[7] = white[MASK4].sum()
	# result = np.zeros(4)
	# result[0] = black[MASK31].sum()
	# result[1] = black[MASK32].sum()
	# result[2] = white[MASK31].sum()
	# result[3] = white[MASK32].sum()
	# fp.write('1:%d 2:%d 3:%d 4:%d\n' % tuple(result))
	# fp.write('1:%d 2:%d 3:%d 4:%d 5:%d 6:%d 7:%d 8:%d\n' % tuple(result))
	tmp = board[x-half_scope:x+(half_scope+1), y-half_scope:y+(half_scope+1)]
	write(fp, c, tmp)
	write(fp, c, tmp.transpose())
	tmp = board[x-half_scope:x+(half_scope+1), y+half_scope:y-(half_scope+1):-1]
	write(fp, c, tmp)
	write(fp, c, tmp.transpose())
	tmp = board[x+half_scope:x-(half_scope+1):-1, y-half_scope:y+(half_scope+1)]
	write(fp, c, tmp)
	write(fp, c, tmp.transpose())
	tmp = board[x+half_scope:x-(half_scope+1):-1, y+half_scope:y-(half_scope+1):-1]
	write(fp, c, tmp)
	write(fp, c, tmp.transpose())

def checkAndRemove(pos_x, pos_y):
	color = board[pos_x, pos_y]
	q.queue.clear()
	q.put((pos_x, pos_y))
	checked[:,:] = False;
	while not q.empty():
		x, y = q.get()
		checked[x, y] = True
		for bias_x,bias_y in [(0,1), (0,-1), (1,0), (-1,0)]:
			xx = x+bias_x
			yy = y+bias_y
			if not checked[xx, yy]:
				if board[xx, yy] == EMPTY_POINT:
					return;
				elif board[xx, yy] == color:
					q.put((xx, yy))
	# print 'eat %d' % checked.sum()
	board[checked] = EMPTY_POINT

def simulate(handicaps, paces, result_fp):
	board[BIAS:-BIAS,BIAS:-BIAS] = EMPTY_POINT
	# fp2 = open('%s.%d.tmp' % (filename, SCOPE), 'w')
	count = 0
	for handicap in handicaps:
		count += 1
		board[handicap[0], handicap[1]] = BLACK_POINT
	for pace in paces:
		count += 1
		# if count > 10:
		saveState(result_fp, pace)
		if board[pace[1], pace[2]] != EMPTY_POINT:
			raise NOT_EMPTY()
		# 	fp2.write('not empty %d %d\n' % (pace[1]-BIAS, pace[2]-BIAS))
		# fp2.write('%d %d\n' % (pace[1]-BIAS, pace[2]-BIAS))
		board[pace[1], pace[2]] = pace[0]
		for bias_x,bias_y in [(0,1), (0,-1), (1,0), (-1,0)]:
			if(board[pace[1]+bias_x, pace[2]+bias_y] == OPPOSE_COLOR[pace[0]]):
				checkAndRemove(pace[1]+bias_x, pace[2]+bias_y)
		checkAndRemove(pace[1], pace[2])
		# for i in range(19):
			# for j in range(19):
				# fp2.write('%s ' % {BLACK_POINT:'X', WHITE_POINT:'O', EMPTY_POINT:'.'}[board[i+BIAS, j+BIAS]])
			# fp2.write('\n')
		# fp2.write('-------------------------------------------------------------------------------\n')
	# fp2.close()

def main():
	global record
	log_fp = open('process.log', 'a')
				# abs_name = r'D:\Sam\Lesson\AI\kgsgo-dataset-preprocessor\data\data\kgs-19-2009-09-new\2009-09-19-32.sgf'
	root = 'D:\\Sam\\Lesson\\AI\\kgsgo-dataset-preprocessor\\data\\data'
	count = 0
	for dirpath, dirnames, filenames in os.walk(root):
		result_fp = open(os.path.join(dirpath, 'result%d.dat' % SCOPE), 'w')
		for filename in filenames:
			# if os.path.splitext(filename)[1] == '.dat':
			# 	count += 1
			# 	if count % 1000 == 0:
			# 		print count
			# 	os.remove(os.path.join(dirpath, filename))
			if (os.path.splitext(filename)[1] == '.sgf'):
				record = False
				count += 1;
				# if count < 50000:
				# 	continue
				if count % 1000 == 0:
					print count, p, save, empty, edge
				# if count > 150:
				# 	return
				abs_name = os.path.join(dirpath, filename)
				# log_fp.write(abs_name)
				# log_fp.flush()
				try:
					fp = open(abs_name, 'r')
					content = re.sub(r'\s', '', fp.read())
					# content = .replace('\n', '\t')
					fp.close()
					readInfo(content, abs_name)
					if not record:
						os.remove(abs_name)
					# handicaps = readHandicap(content)
					# paces = readPaces(content)
					# simulate(handicaps, paces, result_fp)
				except Exception, e:
					print str(e)
					log_fp.write('faled:%s:%s\n' % (str(e), abs_name))
					log_fp.flush()
	log_fp.close()

if __name__ == '__main__':
	q = Queue.Queue()
	bd_re = re.compile(r'BR\[[789]d\]')
	wd_re = re.compile(r'WR\[[789]d\]')
	handicap_re = re.compile(r'\]AB(\[[^;]+)')
	handipos_re = re.compile(r'\[(\w{2})\]')
	pace_re = re.compile(r';([WB])\[(\w{2})\]')
	WHITE_POINT = -1
	BLACK_POINT = 1
	EMPTY_POINT = 0
	EDGE_POINT = 2
	MASK1 = np.array([[1,0,0,0,1],[0,0,0,0,0],[0,0,0,0,0],[0,0,0,0,0],[1,0,0,0,1]], dtype = np.bool)
	MASK2 = np.array([[0,1,0,1,0],[1,0,0,0,1],[0,0,0,0,0],[1,0,0,0,1],[0,1,0,1,0]], dtype = np.bool)
	MASK3 = np.array([[0,0,1,0,0],[0,1,0,1,0],[1,0,0,0,1],[0,1,0,1,0],[0,0,1,0,0]], dtype = np.bool)
	MASK4 = np.array([[0,0,0,0,0],[0,0,1,0,0],[0,1,0,1,0],[0,0,1,0,0],[0,0,0,0,0]], dtype = np.bool)
	MASK31 = np.array([[0,1,0],[1,0,1],[0,1,0]], dtype = np.bool)
	MASK32 = np.array([[1,0,1],[0,0,0],[1,0,1]], dtype = np.bool)
	# record = {WHITE_POINT:False, BLACK_POINT:False}
	OPPOSE_COLOR = {WHITE_POINT:BLACK_POINT, BLACK_POINT:WHITE_POINT}
	BIAS = 4
	SCOPE = 3
	FORMAT = {3:'1:%d 2:%d 3:%d 4:%d 5:%d 6:%d 7:%d 8:%d\n', 5:'1:%d 2:%d 3:%d 4:%d 5:%d 6:%d 7:%d 8:%d 9:%d 10:%d 11:%d 12:%d 13:%d 14:%d 15:%d 16:%d 17:%d 18:%d 19:%d 20:%d 21:%d 22:%d 23:%d 24:%d\n'}[SCOPE]
	board = np.zeros([19+BIAS*2, 19+BIAS*2], dtype = np.int8)
	board[:,:] = EDGE_POINT
	checked = np.zeros([19+BIAS*2, 19+BIAS*2], dtype = np.bool)
	clock()
	# cProfile.run("main()")
	p = 0
	main()
	print p
	print clock()