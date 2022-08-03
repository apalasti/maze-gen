import numpy as np

WALL = (0, 0, 0)
EMPTY = (255, 255, 255)
CORRECT_PATH = (0, 255, 0)
VISITED = (255, 150, 150)


def get_block_size(maze):
    bs = 1
    while np.all(maze[0 : bs + 1, 0 : bs + 1] == maze[0, 0]):
        bs += 1
    return bs


def normalize(maze):
    bs = get_block_size(maze)
    normalized = np.empty((maze.shape[0] // bs, maze.shape[1] // bs, 3), dtype=np.uint8)
    for y in range(normalized.shape[0]):
        for x in range(normalized.shape[1]):
            normalized[y][x] = maze[y * bs][x * bs]
    return normalized


def find_path(maze, start, stop) -> bool:
    if start == stop:
        maze[stop[1], stop[0]] = CORRECT_PATH
        return True

    h, w = maze.shape[0], maze.shape[1]
    for dx, dy in [(1, 0), (-1, 0), (0, 1), (0, -1)]:
        x, y = start[0] + dx, start[1] + dy
        if y < 0 or h <= y or x < 0 or w <= x:
            continue
        if (maze[y, x] == EMPTY).all():
            maze[y, x] = VISITED
            if find_path(maze, (x, y), stop):
                maze[y, x] = CORRECT_PATH
                return True

    return False


def upscale(maze, scale):
    high_res = np.empty((maze.shape[0] * scale, maze.shape[1] * scale, 3), dtype=np.uint8)
    for y in range(maze.shape[0]):
        for x in range(maze.shape[1]):
            high_res[scale * y : scale * (y + 1), scale * x : scale * (x + 1)] = maze[y, x]
    return high_res


if __name__ == "__main__":
    import sys
    from PIL import Image

    sys.setrecursionlimit(10000)

    img = Image.open("maze.bmp")
    maze = np.array(img)
    maze = normalize(maze)

    find_path(maze, (1, 1), (maze.shape[1] - 2, maze.shape[0] - 2))

    solved = Image.fromarray(upscale(maze, 5))
    # solved.save("solution.bmp")
    solved.show()
