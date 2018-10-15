import functools

from BasePacketEditWidget import BasePacketEditWidget


class MotionWidget(BasePacketEditWidget):
    def __init__(self,fd):
        BasePacketEditWidget.__init__(self,fd)
        self.data_check_btn_flag = ((0, 0, 0, 0, 0, 1, 0, 0),
                                    (0, 0, 0, 0, 0, 0, 0, 0),
                                    (0, 0, 0, 0, 0, 0, 0, 0),
                                    (0, 0, 0, 1, 0, 0, 1, 0),
                                    (0, 0, 0, 0, 0, 0, 0, 0),
                                    (0, 0, 0, 0, 0, 0, 0, 0))
        self.data_description = ("  id2 id1 id0   w   0   1   R   L\n"
                                 "L, R = 1 when Left, Right mouse button pressed\n"
                                 "id2..id0 = finger id\n"
                                 "w = 1 when delta overflows (> 127 or < -128), in this case\n"
                                 "firmware sends us (delta x / 5) and (delta y  / 5)",


                                 "     x7  x6  x5  x4  x3  x2  x1  x0\n"
                                 "x7..x0 = delta x (two's complement)",

                                 "     y7  y6  y5  y4  y3  y2  y1  y0\n"
                                 "y7..y0 = delta y (two's complement)",

                                 "    id2 id1 id0   1   0   0   1   0\n"
                                 "id2..id0 = finger id",

                                 "     x7  x6  x5  x4  x3  x2  x1  x0\n"
                                 "x7..x0 = delta x (two's complement)",

                                 "     y7  y6  y5  y4  y3  y2  y1  y0\n"
                                 "y7..y0 = delta y (two's complement)\n"
                                 "byte 0 ~ 2 for one finger\n"
                                 "byte 3 ~ 5 for another",
                                 )

        self.reset_data(self.data_description, self.data_check_btn_flag)
        self.btn_reset.clicked.connect(functools.partial(self.reset_data, data_description=self.data_description,
                                                         data_check_btn_flag=self.data_check_btn_flag))
