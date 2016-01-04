#!/usr/bin/python
# -*- coding:utf8 -*-

input_80 = open("moves_80.dat","r")
input_40 = open("moves_40.dat","r")
input_20 = open("moves_20.dat","r")
input_10 = open("moves_10.dat","r")

rotate_map = {"a":"m","b":"l","c":"k","d":"j",
"e":"i","f":"h","g":"g","h":"f","i":"e","j":"d","k":"c","l":"b","m":"a"}
output_map_1 = {"a":"A","b":"B","c":"C","d":"D","e":"E","f":"F","g":"G","h":"H","i":"J","j":"K","k":"L","l":"M","m":"N"}
output_map_2 = {"a":"1","b":"2","c":"3","d":"4","e":"5","f":"6","g":"7","h":"8","i":"9","j":"10","k":"11","l":"12","m":"13"}
# games_80 = input_80.readlines()
# games_40 = input_40.readlines()
games_20 = input_20.readlines()

def split(s):
	res = []
	for c in s:
		if c == "[" or c == "]":
			continue
		res.append(c)
	return res

def rotate_1(move):
	mv = ["","",""]
	mv[0] = move[0]
	mv[1] = output_map_1[move[1]]
	mv[2] = output_map_2[move[2]]
	mv = mv[1:]
	return ' '.join(mv)
def rotate_2(move):
	mv = ["","",""]
	mv[0] = move[0]
	mv[1] = rotate_map[move[1]]
	mv[1] = output_map_1[mv[1]]
	mv[2] = output_map_2[move[2]]
	mv = mv[1:]
	return ' '.join(mv)

def rotate_3(move):
	mv = ["","",""]
	mv[0] = move[0]
	mv[2] = rotate_map[move[2]]
	mv[1] = output_map_1[move[1]]
	mv[2] = output_map_2[mv[2]]
	return ' '.join(mv[1:])

def rotate_4(move):
	mv = ["","",""]
	mv[0] = move[0]
	mv[1] = rotate_map[move[1]]
	mv[2] = rotate_map[move[2]]
	mv[1] = output_map_1[mv[1]]
	mv[2] = output_map_2[mv[2]]
	return ' '.join(mv[1:])

def static(games):
	moves_dic = {}
	for game in games:
		if "#" in game:
			print game
			continue

		moves = eval(game)
		move_seq  = ""
		move_seq2 = ""
		move_seq3 = ""
		move_seq4 = ""
		cnt = 0
		for move in moves:
			cnt += 1
			move_list = split(move)
			if cnt > 20:
				break
			# print move_list
			try:
				move  = rotate_1(move_list)
				move3 = rotate_3(move_list)
				move2 = rotate_2(move_list)
				move4 = rotate_4(move_list)

				move_seq  += ' '+move
				move_seq2 += ' '+move2
				move_seq3 += ' '+move3
				move_seq4 += ' '+move4
				moves_dic[move_seq]  = moves_dic.get(move_seq ,0) + 1
				moves_dic[move_seq2] = moves_dic.get(move_seq2,0) + 1
				moves_dic[move_seq3] = moves_dic.get(move_seq3,0) + 1
				moves_dic[move_seq4] = moves_dic.get(move_seq4,0) + 1
			except Exception,e:
				print e
				continue
	return moves_dic

def output(moves_dic,output_file):
	print len(moves_dic)
	for item in moves_dic:
		times = moves_dic[item]
		if times < 8:
			continue
		line = str(times)+str(item)+'\n'
		# print line,
		output_file.write(line)

output_20 = open("static_20.dic","w")
output(static(games_20), output_20)
output_20.close()

