from segtypes.n64.rgba16gz import N64SegRgba16gz

class N64SegRgba32gz(N64SegRgba16gz):
    @staticmethod
    def parse_image(data, width, height, flip_h=False, flip_v=False):
        return data

    def max_length(self):
        return self.width * self.height * 4
