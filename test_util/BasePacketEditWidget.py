from PySide2.QtWidgets import QWidget, QHBoxLayout, QCheckBox, QVBoxLayout, QLineEdit, QLabel, QPushButton


class BasePacketEditWidget(QWidget):
    def __init__(self, fd):
        QWidget.__init__(self)
        self.__fd = fd

        self.__HLAYOUT_BYTE = "layout_byte"
        self.__CHECK_BOX_DATA_BIT = "data_byte"

        self.__DESCRIPTION = "description"
        self.init_ui()
        self.btn_send.clicked.connect(self.send_data)

    def init_ui(self):
        names = self.__dict__
        self.main_layout = QVBoxLayout()
        for i in range(6):
            names[self.get_hlayout_name_at(i)] = QHBoxLayout()
            for j in range(8):
                names[self.get_checkbox_at(i, j)] = QCheckBox()
                names[self.get_hlayout_name_at(i)].addWidget(names[self.get_checkbox_at(i, j)])

            self.main_layout.addLayout(names[self.get_hlayout_name_at(i)])
            names[self.__DESCRIPTION + str(i)] = QLabel()
            self.main_layout.addWidget(names[self.__DESCRIPTION + str(i)])
        self.btn_reset = QPushButton("RESET")
        self.btn_send = QPushButton("SEND")
        self.main_layout.addWidget(self.btn_reset)
        self.main_layout.addWidget(self.btn_send)
        self.setLayout(self.main_layout)

    def get_hlayout_name_at(self, i):
        return self.__HLAYOUT_BYTE + str(i)

    def send_data(self):
        data_list = []
        names = self.__dict__
        for i in range(6):
            sum = 0
            for j in range(8):
                sum += (1 if names[self.get_checkbox_at(i, j)].isChecked() else 0) << (7 - j)
            data_list.append(sum)
        data = bytes(data_list)
        if hasattr(self.__fd,'write'):
            writed=self.__fd.write(data)
            self.__fd.flush()
        elif hasattr(self.__fd,'send'):
            writed=self.__fd.send(data)


        print("writed: ",writed)
        print("data: ",data)
        print("Origin:",data_list)

        # TODO send bytes

    def reset_data(self, data_description, data_check_btn_flag):
        names = self.__dict__
        for i in range(6):
            names[self.__DESCRIPTION + str(i)].setText(data_description[i])
            for j in range(8):
                if data_check_btn_flag[i][j] == 1:
                    names[self.get_checkbox_at(i, j)].setChecked(True)
                else:
                    names[self.get_checkbox_at(i, j)].setChecked(False)

    def get_checkbox_at(self, i, j):
        return self.__CHECK_BOX_DATA_BIT + str(i) + str(j)
