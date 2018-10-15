import functools

from BasePacketEditWidget import BasePacketEditWidget


class HeadWidget(BasePacketEditWidget):
    def __init__(self,fd):
        BasePacketEditWidget.__init__(self,fd)
        self.data_check_btn_flag = ((0, 0, 0, 0, 0, 1, 0, 0),
                                    (0, 0, 0, 0, 0, 0, 0, 0),
                                    (0, 0, 0, 0, 0, 0, 0, 0),
                                    (0, 0, 0, 1, 0, 0, 0, 1),
                                    (0, 0, 0, 0, 0, 0, 0, 0),
                                    (0, 0, 0, 0, 0, 0, 0, 0))
        self.data_description = ("w3         w2       w1          w0           0           1          R           L\n"
                                 "L, R = 1 when Left, Right mouse button pressed\n"
                                 "w3..w0 = finger width (spans how many trace lines) ",

                                 "p7  p6  p5  p4 x11 x10  x9  x8",

                                 " x7  x6  x5  x4  x3  x2  x1  x0\n"
                                 "x11..x0 = absolute x value (horizontal)",

                                 "   id2 id1 id0   1   0   0   0   1 \n"
                                 "id2..id0 = finger id",

                                 "     p3  p1  p2  p0  y11 y10 y9  y8\n"
                                 "p7..p0 = pressure",

                                 "     y7  y6  y5  y4  y3  y2  y1  y0\n"
                                 "y11..y0 = absolute y value (vertical)",
                                 )

        self.reset_data(self.data_description, self.data_check_btn_flag)
        self.btn_reset.clicked.connect(functools.partial(self.reset_data, data_description=self.data_description,
                                                         data_check_btn_flag=self.data_check_btn_flag))
