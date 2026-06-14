import sys
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QPushButton, QComboBox, QFrame, QTableWidget, QTableWidgetItem,
    QLineEdit, QTextEdit, QSpinBox, QTabWidget, QProgressBar
)
from PySide6.QtGui import QFont, QIcon
from PySide6.QtCore import Qt, QTimer, Signal, Slot

class SerialPortWidget(QWidget):
    connect_signal = Signal(str)
    disconnect_signal = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self.init_ui()

    def init_ui(self):
        layout = QVBoxLayout()
        layout.setSpacing(10)
        layout.setContentsMargins(10, 10, 10, 10)

        title_label = QLabel("串口连接")
        title_label.setStyleSheet("color: #4a9eff; font-weight: bold; font-size: 14px;")
        layout.addWidget(title_label)

        self.port_combo = QComboBox()
        self.port_combo.addItem("COM7 - USB-SERIAL CH340 v1.0")
        self.port_combo.addItem("COM3 - USB Serial Port")
        self.port_combo.addItem("COM5 - Bluetooth Serial Port")
        self.port_combo.setStyleSheet("""
            QComboBox {
                background-color: #2d3436;
                color: #dfe6e9;
                border: 1px solid #4a69bd;
                border-radius: 4px;
                padding: 5px;
            }
            QComboBox::drop-down {
                border-left: 1px solid #4a69bd;
            }
        """)
        layout.addWidget(self.port_combo)

        button_layout = QHBoxLayout()
        self.refresh_btn = QPushButton("刷新")
        self.refresh_btn.setStyleSheet("""
            QPushButton {
                background-color: #636e72;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 6px 12px;
            }
            QPushButton:hover {
                background-color: #747d8c;
            }
        """)
        self.refresh_btn.clicked.connect(self.refresh_ports)
        button_layout.addWidget(self.refresh_btn)

        self.connect_btn = QPushButton("连接")
        self.connect_btn.setStyleSheet("""
            QPushButton {
                background-color: #0984e3;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 6px 12px;
            }
            QPushButton:hover {
                background-color: #74b9ff;
            }
        """)
        self.connect_btn.clicked.connect(self.on_connect)
        button_layout.addWidget(self.connect_btn)
        layout.addLayout(button_layout)

        status_frame = QFrame()
        status_frame.setStyleSheet("background-color: #2d3436; border-radius: 4px; padding: 10px;")
        status_layout = QVBoxLayout(status_frame)

        status_title = QLabel("设备状态")
        status_title.setStyleSheet("color: #4a9eff; font-weight: bold; font-size: 12px;")
        status_layout.addWidget(status_title)

        self.status_label = QLabel("未连接")
        self.status_label.setStyleSheet("color: #ff7675; font-size: 12px;")
        status_layout.addWidget(self.status_label)

        self.status_desc = QLabel("请连接ESP32设备")
        self.status_desc.setStyleSheet("color: #636e72; font-size: 10px;")
        status_layout.addWidget(self.status_desc)

        layout.addWidget(status_frame)
        layout.addStretch()

        self.setLayout(layout)
        self.setStyleSheet("background-color: #2d3436; border-radius: 8px;")

    def refresh_ports(self):
        pass

    def on_connect(self):
        if self.connect_btn.text() == "连接":
            port = self.port_combo.currentText()
            self.connect_signal.emit(port)
            self.connect_btn.setText("断开")
            self.status_label.setText("已连接")
            self.status_label.setStyleSheet("color: #00cec9; font-size: 12px;")
            self.status_desc.setText(f"已连接到 {port}")
        else:
            self.disconnect_signal.emit()
            self.connect_btn.setText("连接")
            self.status_label.setText("未连接")
            self.status_label.setStyleSheet("color: #ff7675; font-size: 12px;")
            self.status_desc.setText("请连接ESP32设备")

class DeviceScanWidget(QWidget):
    scan_start_signal = Signal(int)
    scan_stop_signal = Signal()
    clear_signal = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self.init_ui()

    def init_ui(self):
        layout = QVBoxLayout()
        layout.setSpacing(15)
        layout.setContentsMargins(15, 15, 15, 15)

        scan_control_frame = QFrame()
        scan_control_frame.setStyleSheet("background-color: #2d3436; border-radius: 8px; padding: 15px;")
        control_layout = QVBoxLayout(scan_control_frame)

        control_title = QLabel("扫描控制")
        control_title.setStyleSheet("color: #4a9eff; font-weight: bold; font-size: 14px; margin-bottom: 10px;")
        control_layout.addWidget(control_title)

        row_layout = QHBoxLayout()
        row_layout.setSpacing(15)

        scan_count_layout = QHBoxLayout()
        scan_count_label = QLabel("扫描数量:")
        scan_count_label.setStyleSheet("color: #dfe6e9;")
        scan_count_layout.addWidget(scan_count_label)

        self.scan_count_spin = QSpinBox()
        self.scan_count_spin.setRange(1, 100)
        self.scan_count_spin.setValue(10)
        self.scan_count_spin.setStyleSheet("""
            QSpinBox {
                background-color: #1e272e;
                color: #dfe6e9;
                border: 1px solid #4a69bd;
                border-radius: 4px;
                padding: 4px;
                width: 60px;
            }
        """)
        scan_count_layout.addWidget(self.scan_count_spin)
        row_layout.addLayout(scan_count_layout)

        self.start_btn = QPushButton("开始扫描")
        self.start_btn.setStyleSheet("""
            QPushButton {
                background-color: #00cec9;
                color: #2d3436;
                border: none;
                border-radius: 4px;
                padding: 6px 16px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: #81ecec;
            }
        """)
        self.start_btn.clicked.connect(self.on_start_scan)
        row_layout.addWidget(self.start_btn)

        self.clear_btn = QPushButton("清空列表")
        self.clear_btn.setStyleSheet("""
            QPushButton {
                background-color: #636e72;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 6px 16px;
            }
            QPushButton:hover {
                background-color: #747d8c;
            }
        """)
        self.clear_btn.clicked.connect(self.on_clear)
        row_layout.addWidget(self.clear_btn)

        control_layout.addLayout(row_layout)
        layout.addWidget(scan_control_frame)

        device_list_frame = QFrame()
        device_list_frame.setStyleSheet("background-color: #2d3436; border-radius: 8px; padding: 15px;")
        list_layout = QVBoxLayout(device_list_frame)

        list_title = QLabel("设备列表")
        list_title.setStyleSheet("color: #4a9eff; font-weight: bold; font-size: 14px; margin-bottom: 10px;")
        list_layout.addWidget(list_title)

        self.device_table = QTableWidget()
        self.device_table.setColumnCount(6)
        self.device_table.setHorizontalHeaderLabels(["序号", "设备名称", "MAC地址", "地址类型", "信号强度", "设备类型"])
        self.device_table.setStyleSheet("""
            QTableWidget {
                background-color: #1e272e;
                color: #dfe6e9;
                border: 1px solid #4a69bd;
                border-radius: 4px;
            }
            QHeaderView::section {
                background-color: #2d3436;
                color: #dfe6e9;
                padding: 8px;
                border: 1px solid #4a69bd;
            }
            QTableWidget::item {
                padding: 8px;
            }
        """)
        self.device_table.setRowCount(0)
        self.device_table.horizontalHeader().setStretchLastSection(True)
        list_layout.addWidget(self.device_table)

        self.empty_label = QLabel("未扫描到蓝牙设备，请点击 [开始扫描]")
        self.empty_label.setStyleSheet("color: #636e72; text-align: center; padding: 40px;")
        self.empty_label.setAlignment(Qt.AlignCenter)
        self.device_table.setCellWidget(0, 0, self.empty_label)

        scan_result_label = QLabel("扫描结果: 0 个设备")
        scan_result_label.setStyleSheet("color: #636e72; font-size: 12px; margin-top: 5px;")
        list_layout.addWidget(scan_result_label)

        layout.addWidget(device_list_frame)

        log_frame = QFrame()
        log_frame.setStyleSheet("background-color: #2d3436; border-radius: 8px; padding: 15px;")
        log_layout = QVBoxLayout(log_frame)

        log_title = QLabel("日志输出")
        log_title.setStyleSheet("color: #4a9eff; font-weight: bold; font-size: 14px; margin-bottom: 10px;")
        log_layout.addWidget(log_title)

        filter_layout = QHBoxLayout()

        time_filter = QComboBox()
        time_filter.addItem("全部")
        time_filter.addItem("最近1小时")
        time_filter.addItem("最近24小时")
        time_filter.setStyleSheet("""
            QComboBox {
                background-color: #1e272e;
                color: #dfe6e9;
                border: 1px solid #4a69bd;
                border-radius: 4px;
                padding: 4px;
                width: 120px;
            }
        """)
        filter_layout.addWidget(QLabel("时间筛选:"))
        filter_layout.addWidget(time_filter)

        filter_layout.addStretch()

        keyword_label = QLabel("关键词搜索:")
        keyword_label.setStyleSheet("color: #dfe6e9;")
        filter_layout.addWidget(keyword_label)

        self.keyword_edit = QLineEdit()
        self.keyword_edit.setPlaceholderText("请输入关键词...")
        self.keyword_edit.setStyleSheet("""
            QLineEdit {
                background-color: #1e272e;
                color: #dfe6e9;
                border: 1px solid #4a69bd;
                border-radius: 4px;
                padding: 4px;
                width: 200px;
            }
        """)
        filter_layout.addWidget(self.keyword_edit)

        search_btn = QPushButton("搜索")
        search_btn.setStyleSheet("""
            QPushButton {
                background-color: #0984e3;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 4px 12px;
            }
        """)
        filter_layout.addWidget(search_btn)

        export_btn = QPushButton("导出日志")
        export_btn.setStyleSheet("""
            QPushButton {
                background-color: #636e72;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 4px 12px;
            }
        """)
        filter_layout.addWidget(export_btn)

        clear_log_btn = QPushButton("清空日志")
        clear_log_btn.setStyleSheet("""
            QPushButton {
                background-color: #636e72;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 4px 12px;
            }
        """)
        filter_layout.addWidget(clear_log_btn)

        log_layout.addLayout(filter_layout)

        self.log_text = QTextEdit()
        self.log_text.setReadOnly(True)
        self.log_text.setStyleSheet("""
            QTextEdit {
                background-color: #1e272e;
                color: #dfe6e9;
                border: 1px solid #4a69bd;
                border-radius: 4px;
                padding: 8px;
            }
        """)
        self.log_text.setPlaceholderText("日志输出区域...")
        log_layout.addWidget(self.log_text)

        layout.addWidget(log_frame)
        layout.addStretch()

        self.setLayout(layout)

    def on_start_scan(self):
        if self.start_btn.text() == "开始扫描":
            count = self.scan_count_spin.value()
            self.scan_start_signal.emit(count)
            self.start_btn.setText("停止扫描")
            self.start_btn.setStyleSheet("""
                QPushButton {
                    background-color: #ff7675;
                    color: white;
                    border: none;
                    border-radius: 4px;
                    padding: 6px 16px;
                    font-weight: bold;
                }
                QPushButton:hover {
                    background-color: #ff8a80;
                }
            """)
            self.add_log("开始扫描蓝牙设备...")
        else:
            self.scan_stop_signal.emit()
            self.start_btn.setText("开始扫描")
            self.start_btn.setStyleSheet("""
                QPushButton {
                    background-color: #00cec9;
                    color: #2d3436;
                    border: none;
                    border-radius: 4px;
                    padding: 6px 16px;
                    font-weight: bold;
                }
                QPushButton:hover {
                    background-color: #81ecec;
                }
            """)
            self.add_log("扫描已停止")

    def on_clear(self):
        self.clear_signal.emit()
        self.device_table.setRowCount(0)
        self.add_log("设备列表已清空")

    def add_log(self, message):
        self.log_text.append(message)

    def add_device(self, index, name, mac, addr_type, rssi, dev_type):
        self.device_table.setRowCount(0)
        row = self.device_table.rowCount()
        self.device_table.insertRow(row)
        self.device_table.setItem(row, 0, QTableWidgetItem(str(index)))
        self.device_table.setItem(row, 1, QTableWidgetItem(name))
        self.device_table.setItem(row, 2, QTableWidgetItem(mac))
        self.device_table.setItem(row, 3, QTableWidgetItem(addr_type))
        self.device_table.setItem(row, 4, QTableWidgetItem(str(rssi)))
        self.device_table.setItem(row, 5, QTableWidgetItem(dev_type))

class RepairToolsWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.init_ui()

    def init_ui(self):
        layout = QVBoxLayout()
        layout.setSpacing(15)
        layout.setContentsMargins(15, 15, 15, 15)

        title = QLabel("修复工具")
        title.setStyleSheet("color: #4a9eff; font-weight: bold; font-size: 18px; margin-bottom: 20px;")
        layout.addWidget(title)

        repair_frame = QFrame()
        repair_frame.setStyleSheet("background-color: #2d3436; border-radius: 8px; padding: 20px;")
        repair_layout = QVBoxLayout(repair_frame)

        btn_style = """
            QPushButton {
                background-color: #0984e3;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 10px 20px;
                font-size: 14px;
                margin: 5px 0;
                text-align: left;
            }
            QPushButton:hover {
                background-color: #74b9ff;
            }
        """

        btns = [
            "设备复位", "固件升级", "清除配对",
            "恢复出厂设置", "校准传感器", "重启设备"
        ]

        for btn_text in btns:
            btn = QPushButton(btn_text)
            btn.setStyleSheet(btn_style)
            repair_layout.addWidget(btn)

        layout.addWidget(repair_frame)
        layout.addStretch()

        self.setLayout(layout)

class BoseTwsWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.init_ui()

    def init_ui(self):
        layout = QVBoxLayout()
        layout.setSpacing(15)
        layout.setContentsMargins(15, 15, 15, 15)

        title = QLabel("Bose TWS 辅助")
        title.setStyleSheet("color: #4a9eff; font-weight: bold; font-size: 18px; margin-bottom: 20px;")
        layout.addWidget(title)

        info_frame = QFrame()
        info_frame.setStyleSheet("background-color: #2d3436; border-radius: 8px; padding: 20px;")
        info_layout = QVBoxLayout(info_frame)

        info_items = [
            ("设备型号", "Bose QC Earbuds"),
            ("固件版本", "5.0.0"),
            ("电池电量", "85%"),
            ("连接状态", "已连接")
        ]

        for label, value in info_items:
            row_layout = QHBoxLayout()
            lbl = QLabel(f"{label}:")
            lbl.setStyleSheet("color: #636e72; width: 100px;")
            val = QLabel(value)
            val.setStyleSheet("color: #dfe6e9;")
            row_layout.addWidget(lbl)
            row_layout.addWidget(val)
            info_layout.addLayout(row_layout)

        layout.addWidget(info_frame)

        control_frame = QFrame()
        control_frame.setStyleSheet("background-color: #2d3436; border-radius: 8px; padding: 20px;")
        control_layout = QVBoxLayout(control_frame)

        btn_style = """
            QPushButton {
                background-color: #6c5ce7;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 10px 20px;
                font-size: 14px;
                margin: 5px 0;
                text-align: left;
            }
            QPushButton:hover {
                background-color: #a29bfe;
            }
        """

        btns = ["同步时间", "同步设置", "读取日志", "发送命令"]
        for btn_text in btns:
            btn = QPushButton(btn_text)
            btn.setStyleSheet(btn_style)
            control_layout.addWidget(btn)

        layout.addWidget(control_frame)
        layout.addStretch()

        self.setLayout(layout)

class LogsWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.init_ui()

    def init_ui(self):
        layout = QVBoxLayout()
        layout.setSpacing(15)
        layout.setContentsMargins(15, 15, 15, 15)

        title = QLabel("日志")
        title.setStyleSheet("color: #4a9eff; font-weight: bold; font-size: 18px; margin-bottom: 20px;")
        layout.addWidget(title)

        log_frame = QFrame()
        log_frame.setStyleSheet("background-color: #2d3436; border-radius: 8px; padding: 15px;")
        log_layout = QVBoxLayout(log_frame)

        self.log_text = QTextEdit()
        self.log_text.setReadOnly(True)
        self.log_text.setStyleSheet("""
            QTextEdit {
                background-color: #1e272e;
                color: #dfe6e9;
                border: 1px solid #4a69bd;
                border-radius: 4px;
                padding: 8px;
            }
        """)
        log_layout.addWidget(self.log_text)

        btn_layout = QHBoxLayout()
        clear_btn = QPushButton("清空日志")
        clear_btn.setStyleSheet("""
            QPushButton {
                background-color: #636e72;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 6px 16px;
            }
        """)
        btn_layout.addWidget(clear_btn)

        export_btn = QPushButton("导出日志")
        export_btn.setStyleSheet("""
            QPushButton {
                background-color: #0984e3;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 6px 16px;
            }
        """)
        btn_layout.addWidget(export_btn)

        btn_layout.addStretch()
        log_layout.addLayout(btn_layout)

        layout.addWidget(log_frame)
        layout.addStretch()

        self.setLayout(layout)

class ProtocolWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.init_ui()

    def init_ui(self):
        layout = QVBoxLayout()
        layout.setSpacing(15)
        layout.setContentsMargins(15, 15, 15, 15)

        title = QLabel("协议抓包")
        title.setStyleSheet("color: #4a9eff; font-weight: bold; font-size: 18px; margin-bottom: 20px;")
        layout.addWidget(title)

        control_frame = QFrame()
        control_frame.setStyleSheet("background-color: #2d3436; border-radius: 8px; padding: 15px;")
        control_layout = QVBoxLayout(control_frame)

        btn_layout = QHBoxLayout()
        self.start_btn = QPushButton("开始抓包")
        self.start_btn.setStyleSheet("""
            QPushButton {
                background-color: #00cec9;
                color: #2d3436;
                border: none;
                border-radius: 4px;
                padding: 8px 24px;
                font-weight: bold;
            }
        """)
        btn_layout.addWidget(self.start_btn)

        self.stop_btn = QPushButton("停止抓包")
        self.stop_btn.setStyleSheet("""
            QPushButton {
                background-color: #ff7675;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 8px 24px;
                font-weight: bold;
            }
        """)
        self.stop_btn.setEnabled(False)
        btn_layout.addWidget(self.stop_btn)

        control_layout.addLayout(btn_layout)

        filter_layout = QHBoxLayout()
        filter_layout.addWidget(QLabel("协议筛选:"))
        filter_combo = QComboBox()
        filter_combo.addItems(["全部", "BLE", "HID", "AVRCP", "A2DP"])
        filter_combo.setStyleSheet("""
            QComboBox {
                background-color: #1e272e;
                color: #dfe6e9;
                border: 1px solid #4a69bd;
                border-radius: 4px;
                padding: 4px;
            }
        """)
        filter_layout.addWidget(filter_combo)
        control_layout.addLayout(filter_layout)

        layout.addWidget(control_frame)

        packet_frame = QFrame()
        packet_frame.setStyleSheet("background-color: #2d3436; border-radius: 8px; padding: 15px;")
        packet_layout = QVBoxLayout(packet_frame)

        self.packet_table = QTableWidget()
        self.packet_table.setColumnCount(5)
        self.packet_table.setHorizontalHeaderLabels(["序号", "时间", "协议", "方向", "数据"])
        self.packet_table.setStyleSheet("""
            QTableWidget {
                background-color: #1e272e;
                color: #dfe6e9;
                border: 1px solid #4a69bd;
                border-radius: 4px;
            }
            QHeaderView::section {
                background-color: #2d3436;
                color: #dfe6e9;
                padding: 8px;
                border: 1px solid #4a69bd;
            }
        """)
        packet_layout.addWidget(self.packet_table)

        layout.addWidget(packet_frame)

        detail_frame = QFrame()
        detail_frame.setStyleSheet("background-color: #2d3436; border-radius: 8px; padding: 15px;")
        detail_layout = QVBoxLayout(detail_frame)

        detail_layout.addWidget(QLabel("数据详情:"))
        self.detail_text = QTextEdit()
        self.detail_text.setReadOnly(True)
        self.detail_text.setStyleSheet("""
            QTextEdit {
                background-color: #1e272e;
                color: #dfe6e9;
                border: 1px solid #4a69bd;
                border-radius: 4px;
                padding: 8px;
                font-family: monospace;
            }
        """)
        detail_layout.addWidget(self.detail_text)

        layout.addWidget(detail_frame)
        layout.addStretch()

        self.setLayout(layout)

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("ESP32 TWS 蓝牙耳机修复工具 v1.0.0")
        self.setGeometry(100, 100, 1200, 800)
        self.setStyleSheet("background-color: #1e272e;")
        self.init_ui()

    def init_ui(self):
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QHBoxLayout(central_widget)
        main_layout.setSpacing(15)
        main_layout.setContentsMargins(15, 15, 15, 15)

        left_panel = QFrame()
        left_panel.setStyleSheet("background-color: #2d3436; border-radius: 8px;")
        left_panel.setFixedWidth(280)
        left_layout = QVBoxLayout(left_panel)

        title_label = QLabel("ESP32 TWS 蓝牙耳机修复工具")
        title_label.setStyleSheet("color: #00cec9; font-weight: bold; font-size: 16px; padding: 15px;")
        left_layout.addWidget(title_label)

        serial_widget = SerialPortWidget()
        left_layout.addWidget(serial_widget)

        version_label = QLabel("v1.0.0")
        version_label.setStyleSheet("color: #636e72; font-size: 12px; padding: 10px; text-align: right;")
        left_layout.addWidget(version_label)

        main_layout.addWidget(left_panel)

        right_panel = QFrame()
        right_panel.setStyleSheet("background-color: #2d3436; border-radius: 8px;")
        right_layout = QVBoxLayout(right_panel)

        tab_widget = QTabWidget()
        tab_widget.setStyleSheet("""
            QTabWidget::tab-bar {
                alignment: left;
            }
            QTabBar::tab {
                background-color: #1e272e;
                color: #636e72;
                padding: 10px 20px;
                margin-right: 5px;
                border-radius: 4px 4px 0 0;
            }
            QTabBar::tab:selected {
                background-color: #0984e3;
                color: white;
            }
            QTabWidget::pane {
                background-color: #1e272e;
                border: none;
            }
        """)

        self.device_scan_widget = DeviceScanWidget()
        self.repair_tools_widget = RepairToolsWidget()
        self.bose_tws_widget = BoseTwsWidget()
        self.logs_widget = LogsWidget()
        self.protocol_widget = ProtocolWidget()

        tab_widget.addTab(self.device_scan_widget, "设备扫描")
        tab_widget.addTab(self.repair_tools_widget, "修复工具")
        tab_widget.addTab(self.bose_tws_widget, "Bose TWS 辅助")
        tab_widget.addTab(self.logs_widget, "日志")
        tab_widget.addTab(self.protocol_widget, "协议抓包")

        right_layout.addWidget(tab_widget)
        main_layout.addWidget(right_panel)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
