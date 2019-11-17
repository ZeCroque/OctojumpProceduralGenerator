from random import *

import numpy as np
from PIL import Image


class Coord:
    def __init__(self, s):
        self.x = randint(round(s * 0.10), round(s * 0.90))
        self.y = randint(round(s * 0.10), round(s * 0.90))


class IMapManager:
    def __init__(self, size):
        self.size = size
        self.i_map = [[1 for x in range(self.size)] for y in range(self.size)]
        self.image = np.zeros((size, size, 3), dtype=np.uint8)

        self.tolerance = np.clip(round(self.size * 0.01), 1, 10)

        self.treated_pixel = []

    def new_coord(self):
        coord = Coord(self.size)
        return coord

    def __have_to_close_parallel_right(self, x, y, end_line):
        for y in range(int(np.clip(y - self.tolerance, 0, self.size - 1)),
                       int(np.clip(y + self.tolerance, 0, self.size - 1))):
            last = -1
            for j in range(x, end_line):
                if self.i_map[y][j] == 0 and last == 0:
                    return True
                last = self.i_map[y][j]
        return False

    def __have_to_close_parallel_down(self, x, y, end_line):
        for x in range(int(np.clip(x - self.tolerance, 0, self.size - 1)),
                       int(np.clip(x + self.tolerance, 0, self.size - 1))):
            last = -1
            for j in range(y, end_line):
                if self.i_map[j][x] == 0 and last == 0:
                    return True
                last = self.i_map[j][x]
        return False

    def __have_to_close_parallel_left(self, x, y, end_line):
        for y in range(int(np.clip(y - self.tolerance, 0, self.size - 1)),
                       int(np.clip(y + self.tolerance, 0, self.size - 1))):
            last = -1
            for j in range(x, end_line, -1):
                if self.i_map[y][j] == 0 and last == 0:
                    return True
                last = self.i_map[y][j]
        return False

    def __have_to_close_parallel_up(self, x, y, end_line):
        for x in range(int(np.clip(x - self.tolerance, 0, self.size - 1)),
                       int(np.clip(x + self.tolerance, 0, self.size - 1))):
            last = -1
            for j in range(y, end_line, -1):
                if self.i_map[j][x] == 0 and last == 0:
                    return True
                last = self.i_map[j][x]
        return False

    def new_cross(self):
        coord = self.new_coord()
        checker = 0

        end_line_right = round(coord.x + (self.size - coord.x) * 0.60) + randint(0, round((self.size - coord.x) * 0.30))
        if not self.__have_to_close_parallel_right(coord.x, coord.y, end_line_right):
            checker += 0.33
            for j in range(coord.x, end_line_right):
                self.i_map[coord.y][j] = 0

        end_line_down = round(coord.y + (self.size - coord.y) * 0.60) + randint(0, round((self.size - coord.y) * 0.30))
        if not self.__have_to_close_parallel_down(coord.x, coord.y, end_line_down):
            checker += 0.33
            for j in range(coord.y, end_line_down):
                self.i_map[j][coord.x] = 0

        end_line_left = coord.x - (round(coord.x * 0.60) + randint(0, round(coord.x * 0.30)))
        if not self.__have_to_close_parallel_left(coord.x, coord.y, end_line_left):
            checker += 0.33
            for j in range(coord.x, end_line_left, -1):
                self.i_map[coord.y][j] = 0

        end_line_up = coord.y - (round(coord.y * 0.60) + randint(0, round(coord.y * 0.30)))
        if not self.__have_to_close_parallel_up(coord.x, coord.y, end_line_up):
            checker += 0.33
            for j in range(coord.y, end_line_up, -1):
                self.i_map[j][coord.x] = 0

        if checker > 0:
            self.i_map[coord.y][coord.x] = 2

        return checker

    def image_maker(self):
        self.image = np.zeros((self.size - 1, self.size - 1, 3), dtype=np.uint8)
        for x in range(self.size - 1):
            for y in range(self.size - 1):
                if self.i_map[x][y] == 1:
                    self.image[x][y] = [0, 0, 0]
                elif self.i_map[x][y] == 0:
                    self.image[x][y] = [255, 255, 255]
                elif self.i_map[x][y] == 2:
                    self.image[x][y] = [255, 0, 0]
        return self.image

    def new_pixel(self, coord):
        pass

    def clean_map(self):
        pixel_to_do = [[0, 0]]
        s = self.size - 1
        for coord in pixel_to_do:
            x = coord[0]
            y = coord[1]
            self.i_map[x][y] = 0
            for n_x in range(-1, 2):
                for n_y in range(-1, 2):
                    new_pix = [int(np.clip(x + n_x, 0, s)), int(np.clip(y + n_y, 0, s))]
                    if new_pix not in pixel_to_do and self.i_map[new_pix[0]][new_pix[1]] == 1:
                        pixel_to_do.append(new_pix)


def main():
    s = 150
    im_manager = IMapManager(s)

    i = 0
    while i < round(s * 0.30):
        i += im_manager.new_cross()

    #im_manager.clean_map()

    image = im_manager.image_maker()
    img = Image.fromarray(image, 'RGB')
    img.save('my.png')
    img.show()


if __name__ == '__main__':
    main()
