class Grid:
	"""
		Args:
			width (int): Grid's width.
			height (int): Grid's height.

		Attributes:
			grid (list): The list will hold the position empty or not information.
			width (int): Grid's width
			height (int): Grid's height
	"""

	def __init__(self, width, height):
		self.grid = [False] * (width * height)
		self.itemPos = [0] * (10*10)
		self.width = width
		self.height = height

	def __str__(self):
		output = "Grid {}x{} Information\n".format(self.width, self.height)
		for row in range(self.height):
			for col in range(self.width):
				output += "Status of %d: " % (row * self.width + col)
				output += "NotEmpty, " if self.grid[row *
												   self.width + col] else "Empty, "
			output += "\n"

		return output

	def find_blank(self, width, height):
		"""
			Args:
				width (int): The item's width you can call width item.GetItemSize()[0]
				height (int): The item's height you can call width item.GetItemSize()[1]

			Returns:
				int: The return value would be an int if successful. Otherwise -1.
		"""
		if width > self.width or height > self.height:
			return -1

		for row in range(self.height):
			for col in range(self.width):
				index = row * self.width + col
				if self.is_empty(index, width, height):
					return index

		return -1

	def put(self, pos, width, height, realPos = 0):
		"""
			Args:
				pos (int): Position of the item to put.
				width (int): The item's width you can call width item.GetItemSize()[0]
				height (int): The item's height you can call width item.GetItemSize()[1]

			Returns:
				bool: The return value. True for success, False otherwise.
		"""
		if not self.is_empty(pos, width, height):
			return False

		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = True
			self.itemPos[start] = realPos
			col = 1
			while col < width:
				self.grid[start + col] = True
				self.itemPos[start + col] = realPos
				col += 1

		return True

	def clear(self, pos, width, height, realPos = 0):
		"""
			Args:
				pos (int): Position of the item to put.
				width (int): The item's width you can call width item.GetItemSize()[0]
				height (int): The item's height you can call width item.GetItemSize()[1]

			Returns:
				There is nothing to return
		"""
		if pos < 0 or pos >= (self.width * self.height):
			return

		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = False
			self.itemPos[start] = realPos
			col = 1
			while col < width:
				self.grid[start + col] = False
				self.itemPos[start + col] = 0
				col += 1

	def is_empty(self, pos, width, height):
		"""
			Args:
				pos (int): Position of the item to put.
				width (int): The item's width you can call width item.GetItemSize()[0]
				height (int): The item's height you can call width item.GetItemSize()[1]

			Returns:
				bool: The return value. True for success, False otherwise.
		"""
		if pos < 0:
			return False

		row = pos // self.width
		if (row + height) > self.height:
			return False

		if (pos + width) > ((row * self.width) + self.width):
			return False

		for row in range(height):
			start = pos + (row * self.width)            
			if self.grid[start]:                
				return False

			col = 1
			while col < width:
				if self.grid[start + col]:                    
					return False
				col += 1

		return True

	def get_size(self):
		"""
			Returns:
				int: The return value will give you maximum capacity of grid. (width * height)
		"""
		return self.width * self.height

	def reset(self):
		"""
			With this function, you can reset instead of deleting it and create again.
		"""
		self.grid = [False] * (self.width * self.height)
		
	def getRealPos(self, pos):
		return self.itemPos[pos]

'''
C to Python For RGames []
'''

class PythonGrid:
	def __init__(self, width, height):
		self.ReSize(width, height)

	def Clear(self):
		self.m_pGrid = [0 for i in xrange(self.m_iWidth * self.m_iHeight)]

	def ReSize(self, width, height):
		self.m_iWidth = width
		self.m_iHeight = height
		self.m_pGrid = [0 for i in xrange(self.m_iWidth * self.m_iHeight)]

	def FindBlank(self, w, h, array = []):
		if (w > self.m_iWidth or h > self.m_iHeight):
			return -1;
		iRow = 0
		while iRow < self.m_iHeight:
			iCol = 0
			while iCol < self.m_iWidth:
				iIndex = iRow * self.m_iWidth + iCol
				if self.IsEmpty(iIndex, w, h):
					if (len(array) == 0):
						return iIndex
					elif not iIndex in array:
						return iIndex
				iCol += 1
			iRow += 1
		return -1

	def Put(self, iPos, w, h):
		if not self.IsEmpty(iPos, w, h):
			return False
		y = 0
		while y < h:
			iStart = iPos + (y*self.m_iWidth)
			self.m_pGrid[iStart] = 1
			x = 1
			while x < w:
				self.m_pGrid[iStart + x] = 1
				x += 1
			y += 1
		return True

	def IsEmpty(self, iPos, w, h):
		if iPos < 0:
			return False
		iRow = iPos / self.m_iWidth
		if (iRow + h) > self.m_iHeight:
			return False
		if (iPos + w) > (iRow * self.m_iWidth + self.m_iWidth):
			return False
		y = 0
		while y < h:
			iStart = iPos + (y * self.m_iWidth)
			if (self.m_pGrid[iStart]):
				return False
			x = 1
			while (x < w):
				if self.m_pGrid[iStart + x]:
					return False
				x += 1
			y += 1
		return True

	def Get(self, iPos, w, h):
		if (iPos < 0 or iPos >= self.m_iWidth * self.m_iHeight):
			return
		y = 0
		while y < h:
			iStart = iPos + (y * self.m_iWidth)
			self.m_pGrid[iStart] = 0
			x = 1
			while x < w:
				self.m_pGrid[iStart + x] = 0
				x += 1
			y += 1

	def Print(self):
		import dbg
		dbg.TraceError("Grid \n")
		y = 0
		while y < self.m_iHeight:
			x = 0
			while x < self.m_iWidth:
				dbg.TraceError("%d %d" % (y * self.m_iWidth + x, self.m_pGrid[y * self.m_iWidth + x]))
				x += 1
			y += 1
