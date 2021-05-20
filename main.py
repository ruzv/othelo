import pygame
import sys


class Othelo:

	game_board = []
	margin = 5
	square = 0

	def __init__(self, s):
		self.current_player = 0 #white
		self.screen_size = s
		self.square = int((s - (2*self.margin)) / 8)
		pygame.init()
		self.screen = pygame.display.set_mode([s, s], vsync=1)
		pygame.display.set_caption("Othelo")
		self.init_game_board();

	def start(self):
		self.draw()
		while 1:
			self.player()
			self.draw()
			pygame.time.delay(400)

			self.bot1()
			self.draw()
			pygame.time.delay(400)


	def draw(self):
		self.screen.fill([40, 40, 40])
		self.draw_game_board()
		pygame.display.flip()

	def player(self):
		while 1:
			for event in pygame.event.get():
				if event.type == pygame.QUIT:
					sys.exit()
				elif(event.type == pygame.MOUSEBUTTONDOWN):
					pos = self.get_board_pos(event.pos)
					if(self.move_changes(pos) > 0):
						self.apply_move(pos)
						return
					else:
						print("invalid move")

	def bot0(self):
		for y in range(8):
			for x in range(8):
				if(self.move_changes([x, y]) > 0):
					self.apply_move([x, y])
					return

	def bot1(self):
		best_move = None
		best_move_changes = 0
		for y in range(8):
			for x in range(8):
				c = self.move_changes([x, y])
				if(c > best_move_changes):
					best_move_changes = c
					best_move = [x, y]
		if(best_move != None):
			self.apply_move(best_move)

	def bot2(self):
		pass


	def place_stone(self, pos):

		if(self.move_changes(pos) > 0):
			self.apply_move(pos)
		else:
			print("invalid move")
			return
		self.draw()
		pygame.time.delay(1)

		best_move = [0, 0]
		best_move_changes = 0
		for y in range(8):
			for x in range(8):
				c = self.move_changes([x, y])
				if(c > best_move_changes):
					best_move_changes = c
					best_move = [x, y]
		self.apply_move(best_move)
		self.draw()

	def move_changes(self, pos):
		if(self.game_board[pos[1]][pos[0]] != 0):
			return 0

		changes = 0
		for dy in [-1, 0, 1]:
			for dx in [-1, 0, 1]:
				if(not(dx == 0 and dy == 0)):
					changes += self.line(pos, [dx, dy])
		return changes

	def is_valid_move(self, pos):
		if(self.game_board[pos[1]][pos[0]] != 0):
			return False

		changes = 0

		for dy in [-1, 0, 1]:
			for dx in [-1, 0, 1]:
				if(not(dx == 0 and dy == 0)):
					changes += self.line(pos, [dx, dy])

		if(changes == 0):
			return False
		else:
			return True

	def apply_move(self, pos):
		for dy in [-1, 0, 1]:
			for dx in [-1, 0, 1]:
				if(not(dx == 0 and dy == 0)):
					p = pos[:]
					for i in range(self.line(pos, [dx, dy])):
						p[0] += dx
						p[1] += dy
						self.game_board[p[1]][p[0]] = 1 + self.current_player
		self.game_board[pos[1]][pos[0]] = 1 + self.current_player
		self.current_player = (self.current_player+1)%2


	def line(self, p, dire):
		pos = p[:]
		count = 0
		while True:
			pos[0] += dire[0]
			pos[1] += dire[1]
			if(pos[0] >= 0 and pos[0] <= 7 and pos[1] >= 0 and pos[1] <= 7):
				if(self.game_board[pos[1]][pos[0]] == (1+((self.current_player+1)%2))):
					count += 1
				elif(self.game_board[pos[1]][pos[0]] == 0):
					return 0
				elif(self.game_board[pos[1]][pos[0]] == 1+self.current_player):
					return count
			else:
				return 0


	def init_game_board(self):
		for y in range(8):
			row = []
			for x in range(8):
				row.append(0)
			self.game_board.append(row)
		self.game_board[3][3] = 1
		self.game_board[4][4] = 1
		self.game_board[3][4] = 2
		self.game_board[4][3] = 2

	def draw_game_board(self):
		for y in range(8):
			for x in range(8):
				pygame.draw.rect(self.screen, [100, 100, 100], [self.margin+(self.square*x), self.margin+(self.square*y), self.square, self.square], 1)
				if(self.game_board[y][x] == 1):
					pygame.draw.circle(self.screen, (255, 255, 255), [self.margin+(self.square*x)+(self.square/2), self.margin+(self.square*y)+(self.square/2)], self.square*0.4)
				elif(self.game_board[y][x] == 2):
					pygame.draw.circle(self.screen, (0, 0, 0), [self.margin+(self.square*x)+(self.square/2), self.margin+(self.square*y)+(self.square/2)], self.square*0.4)

	def get_board_pos(self, pos):
		x = pos[0]
		y = pos[1]
		x = int((x - self.margin) / self.square)
		y = int((y - self.margin) / self.square)
		return [x, y]


ot = Othelo(600)
ot.start()