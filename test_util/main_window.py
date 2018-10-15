import socket
import sys

from PySide2.QtCore import Qt
from PySide2.QtWidgets import QApplication, QLabel, QWidget, QPushButton, QVBoxLayout, QComboBox, QLineEdit, \
    QMessageBox

from HeadWidget import HeadWidget
from Motion import MotionWidget
from StatuWidget import StatusWidget


class MyWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        self.init_ui()

        self.combobox.currentTextChanged.connect(self.slot_combo_textchanged)
        self.btn_open_conn.clicked.connect(self.slot_conn_clicked)
        self.btn_close_conn.clicked.connect(self.slot_conn_close)
        self.btn_status.clicked.connect(self.slot_send_status_packet)
        self.btn_head.clicked.connect(self.slot_send_head_packet)
        self.btn_motion.clicked.connect(self.slof_send_motion_packet)

    def slot_conn_close(self):
        #TODO 关闭其他窗口
        self.fd.close()
        self.set_send_btn_statu(False)

    def slot_send_status_packet(self):
        statuswidget = StatusWidget(self.fd)
        statuswidget.show()

    def slot_send_head_packet(self):
        headwidget=HeadWidget(self.fd)
        headwidget.show()

    def slof_send_motion_packet(self):
        motionwidget=MotionWidget(self.fd)
        motionwidget.show()
        pass

    def init_ui(self):
        self.tuple_method = ("Connect to Server", "Connect to /dev")
        self.combobox = QComboBox()
        self.combobox.addItems(self.tuple_method)
        self.addr_edit = QLineEdit()
        self.port_edit = QLineEdit()
        self.btn_open_conn = QPushButton("Connect/Open")
        self.btn_close_conn= QPushButton("Close Conn")
        self.fd = None
        self.method_layout = QVBoxLayout()
        self.method_layout.addWidget(self.combobox)
        self.method_layout.addWidget(self.addr_edit)
        self.method_layout.addWidget(self.port_edit)
        self.method_layout.addWidget(self.btn_open_conn)
        self.method_layout.addWidget(self.btn_close_conn)
        self.btn_status = QPushButton("Send Status Packet")
        self.btn_head = QPushButton("Send Head Packet")
        self.btn_motion = QPushButton("Send Motion Packet")
        self.packet_layout = QVBoxLayout()
        self.packet_layout.addWidget(self.btn_status)
        self.packet_layout.addWidget(self.btn_head)
        self.packet_layout.addWidget(self.btn_motion)
        self.main_layout = QVBoxLayout()
        self.main_layout.addLayout(self.method_layout)
        self.main_layout.addLayout(self.packet_layout)
        self.setLayout(self.main_layout)
        self.set_send_btn_statu(False)

    def set_send_btn_statu(self, statu):
        self.btn_motion.setEnabled(statu)
        self.btn_head.setEnabled(statu)
        self.btn_status.setEnabled(statu)
        self.btn_close_conn.setEnabled(statu)
        self.btn_open_conn.setEnabled(True if statu==False else False)

    def slot_combo_textchanged(self, current_text):
        if current_text == "Connect to /dev":
            self.port_edit.setVisible(False)
        else:
            self.port_edit.setVisible(True)

    def slot_conn_clicked(self):
        # try:
        if self.combobox.currentText() == "Connect to /dev":
            self.fd = open(self.addr_edit.text(), "wb")

        elif self.combobox.currentText() == "Connect to Server":
            self.fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.fd.connect(("127.0.0.1", int(self.port_edit.text())))
        # except :
        #     pass

        # TODO except

        QMessageBox.information(self, "success", "connected")
        self.set_send_btn_statu(True)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = MyWidget()
    # widget=StatusWidget()
    widget.show()
    sys.exit(app.exec_())
