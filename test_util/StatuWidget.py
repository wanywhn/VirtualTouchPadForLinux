import functools

from BasePacketEditWidget import BasePacketEditWidget


class StatusWidget(BasePacketEditWidget):
    def __init__(self, fd):
        BasePacketEditWidget.__init__(self, fd)
        self.data_check_btn_flag = ((0, 0, 0, 0, 0, 1, 0, 0),
                                    (0, 0, 0, 0, 0, 0, 0, 0),
                                    (0, 0, 0, 0, 0, 0, 0, 0),
                                    (0, 0, 0, 1, 0, 0, 0, 0),
                                    (0, 0, 0, 0, 0, 0, 0, 0),
                                    (0, 0, 0, 0, 0, 0, 0, 0))
        self.data_description = (".            .             .            .            0           1            R "
                                 "L\nL, R = 1 when Left, Right mouse button pressed",
                                 ".   .   . ft4 ft3 ft2 ft1 ft0\nft4 ft3 ft2 ft1 ft0 ftn = 1 when finger n is on touchpad",
                                 "not used",
                                 ".   .   .   1   0   0   0   0\nconstant bits",
                                 "p   .   .   .   .   .   .   .\np = 1 for palm",
                                 "not used",
                                 )

        self.reset_data(self.data_description, self.data_check_btn_flag)
        self.btn_reset.clicked.connect(functools.partial(self.reset_data, data_description=self.data_description,
                                                         data_check_btn_flag=self.data_check_btn_flag))
