#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  initializer();

  auto *groupBox = new QGroupBox(tr("Targets"));
  groupBox->setAlignment(Qt::AlignTop);
  auto *vbox = new QGridLayout;
  vbox->addWidget(combobox_programmer, 0, 0, Qt::AlignTop);
  vbox->addWidget(btn_check_programmer, 0, 1);
  connect(btn_check_programmer, SIGNAL (released()), this, SLOT (check_for_programmer()));
  vbox->addWidget(button_update, 0, 2);
  connect(button_update, SIGNAL (released()), this, SLOT (update_firmware()));
  vbox->addWidget(combobox_device, 1, 0, 2, 0);
  connect(combobox_device, SIGNAL (currentTextChanged(QString)), this, SLOT (select_device(QString)));
  groupBox->setLayout(vbox);
  layout->addWidget(groupBox, 0, 0);

  auto *groupBox3 = new QGroupBox(tr("Actions"));
  groupBox3->setAlignment(Qt::AlignTop);
  auto *vbox3 = new QGridLayout;
  vbox3->addWidget(pin_check, 0, 0, Qt::AlignTop);
  vbox3->addWidget(blank_check, 0, 1);
  vbox3->addWidget(erase_device, 1, 0, Qt::AlignTop);
  vbox3->addWidget(hardware_check, 1, 1);
  vbox3->addWidget(button_run_command, 2, 0, 1, 0);
  connect(button_run_command, SIGNAL (released()), this, SLOT (run_command()));
  vbox3->addWidget(no_id_error, 4, 0);
  vbox3->addWidget(skip_id, 5, 0);
  vbox3->addWidget(no_size_error, 4, 1);
  vbox3->addWidget(skip_verify, 5, 1);
  vbox3->addWidget(button_read, 6, 0);
  connect(button_read, SIGNAL (released()), this, SLOT (read_device()));
  vbox3->addWidget(button_write, 6, 1);
  connect(button_write, SIGNAL (released()), this, SLOT (write_device()));
  groupBox3->setLayout(vbox3);
  layout->addWidget(groupBox3, 2, 0);

  auto *groupBox4 = new QGroupBox(tr("Device Info"));
  auto *vbox4 = new QGridLayout;
  vbox4->addWidget(device_name_label, 0, 0);
  vbox4->addWidget(device_name, 0, 1);
  vbox4->addWidget(device_memory_label, 1, 0);
  vbox4->addWidget(device_memory, 1, 1);
  vbox4->addWidget(device_package_label, 2, 0);
  vbox4->addWidget(device_package, 2, 1);
  vbox4->addWidget(device_protocol_label, 0, 2);
  vbox4->addWidget(device_protocol, 0, 3);
  vbox4->addWidget(device_readbuffer_label, 1, 2);
  vbox4->addWidget(device_readbuffer, 1, 3);
  vbox4->addWidget(device_writebuffer_label, 2, 2);
  vbox4->addWidget(device_writebuffer, 2, 3);
  groupBox4->setLayout(vbox4);
  layout->addWidget(groupBox4, 0, 1);

  auto *groupBox5 = new QGroupBox(tr("Hex Viewer"));
  auto *vbox5 = new QVBoxLayout;

  hexTableView->setModel(&hexViewModel);
  hexTableView->show();
  vbox5->addWidget(hexTableView);
  vbox5->addStretch(1);
  groupBox5->setLayout(vbox5);
  layout->addWidget(groupBox5, 2, 1);

  auto *groupBox7 = new QGroupBox(tr("System Settings"));
  auto *vbox7 = new QGridLayout;
  vbox7->addWidget(system_sett_path_label, 1, 1);
  vbox7->addWidget(system_sett_path, 1, 2);
  vbox7->addWidget(button_select_path, 1, 3);
  connect(button_select_path, SIGNAL (released()), this, SLOT (select_path_dialog()));
  vbox7->addWidget(button_test_minipro_path, 1, 4);
  connect(button_test_minipro_path, SIGNAL (released()), this, SLOT (check_for_minipro()));
  vbox7->addWidget(output_path_label, 2, 1);
  vbox7->addWidget(output_path, 2, 2);
  vbox7->addWidget(button_select_output_path, 2, 3);
  connect(button_select_output_path, SIGNAL (released()), this, SLOT (select_output_path_dialog()));
  groupBox7->setLayout(vbox7);
  layout->addWidget(groupBox7, 3, 0, 1, 0);

  auto *groupBox6 = new QGroupBox(tr("Output"));
  auto *vbox6 = new QVBoxLayout;
  vbox6->addWidget(status_view);
  vbox6->addStretch(1);
  groupBox6->setLayout(vbox6);
  layout->addWidget(groupBox6, 4, 0, 1, 0);

  status_view->setReadOnly(true);

  disable_buttons();
  check_for_minipro();
  build_default_hex_output();

  window->setWindowIcon(QIcon(":/res/AppIcon.png"));
  window->show();
}

MainWindow::~MainWindow() = default;

void MainWindow::initializer() {
  minipro_found = false;
  programmer_found = false;

  window = new QWidget;
  window->setWindowTitle("minipro CLI not found!");
  window->resize(1000, 600);
  window->setMinimumSize(800, 480);

  layout = new QGridLayout(window);

  combobox_programmer = new QComboBox();
  combobox_device = new QComboBox();

  button_run_command = new QPushButton("Run Command");
  button_write = new QPushButton("Write to Device");
  button_read = new QPushButton("Read from Device");
  button_update = new QPushButton("Update Firmware");
  button_test_minipro_path = new QPushButton("Test minipro path");
  button_select_path = new QPushButton("Select path (dialog)");
  button_select_output_path = new QPushButton("Set output file");
  btn_check_programmer = new QPushButton("Refresh list");

  no_id_error = new QCheckBox("Ignore ID Error");
  skip_id = new QCheckBox("Skip ID Check");
  no_size_error = new QCheckBox("Ignore Size Error");
  skip_verify = new QCheckBox("Skip Verification");
  pin_check = new QCheckBox("Pin Check");
  blank_check = new QCheckBox("Blank Check");
  erase_device = new QCheckBox("Erase Device");
  hardware_check = new QCheckBox("Hardware Check");

  device_name_label = new QLabel("Name");
  device_name = new QLineEdit();
  device_name->setReadOnly(true);
  device_memory_label = new QLabel("Memory");
  device_memory = new QLineEdit();
  device_memory->setReadOnly(true);
  device_package_label = new QLabel("Package");
  device_package = new QLineEdit();
  device_package->setReadOnly(true);
  device_protocol_label = new QLabel("Protocol");
  device_protocol = new QLineEdit();
  device_protocol->setReadOnly(true);
  device_readbuffer_label = new QLabel("Read Buffer");
  device_readbuffer = new QLineEdit();
  device_readbuffer->setReadOnly(true);
  device_writebuffer_label = new QLabel("Write Buffer");
  device_writebuffer = new QLineEdit();
  device_writebuffer->setReadOnly(true);

  system_sett_path_label = new QLabel("minipro Path");
  system_sett_path = new QLineEdit();
  system_sett_path->setReadOnly(false);
  QString path_val = get_preferences("minipro_path");
  if(path_val != "null")
    system_sett_path->setText(path_val);
  else
    system_sett_path->setText("Enter minipro path...");
  output_path_label = new QLabel("Set output file");
  output_path = new QLineEdit();
  output_path->setReadOnly(false);

  hexTableView = new QTableView(window);
  status_view = new QPlainTextEdit(window);

  monospace_font.setFamily("Courier New");
  monospace_font.setStyleHint(QFont::Monospace);
  status_view->setFont(monospace_font);
}

void MainWindow::run_async_process(QStringList &process_arguments, const QString &type = "stderr") {
  QString minipro_path = system_sett_path->text().trimmed();
  if (minipro_path.isEmpty() || minipro_path.contains("Enter minipro path")) {
      minipro_path = "minipro";
  }
  QString process_arguments_string = "";
  async_process = new QProcess();
  for (auto const &each : process_arguments) {
    process_arguments_string += each + " ";
  }
  for (auto const &each : parse_checkboxes()) {
    process_arguments_string += each + " ";
    process_arguments.append(each);
  }

  status_view->appendPlainText("[Input]: minipro " + process_arguments_string);
  if (type == "stderr") {
    connect(async_process, SIGNAL(readyReadStandardError()), this, SLOT(async_process_err_output()));
  } else if (type == "stdout") {
    connect(async_process, SIGNAL(readyReadStandardOutput()), this, SLOT(async_process_std_output()));
  }
  connect(async_process,
          SIGNAL(finished(int,QProcess::ExitStatus)),
          this,
          SLOT(read_device_output(int,QProcess::ExitStatus)));

  status_view->appendPlainText("[Output]: ");
  status_view->ensureCursorVisible();
  async_process->start(minipro_path, process_arguments);
}

void MainWindow::async_process_err_output() const {
  status_view->appendPlainText(async_process->readAllStandardError().trimmed().replace("\u001B[K", ""));
  status_view->ensureCursorVisible();
}

void MainWindow::async_process_std_output() const {
  status_view->appendPlainText(async_process->readAllStandardOutput().trimmed().replace("\u001B[K", ""));
  status_view->ensureCursorVisible();
}

QStringList MainWindow::parse_checkboxes() const {
  QStringList arguments;
  if (no_id_error->isChecked()) arguments.append("--no_id_error");
  if (skip_id->isChecked()) arguments.append("--skip_id");
  if (no_size_error->isChecked()) arguments.append("--no_size_error");
  if (skip_verify->isChecked()) arguments.append("--skip_verify");
  if (pin_check->isChecked()) arguments.append("--pin_check");
  if (blank_check->isChecked()) arguments.append("--blank_check");
  if (erase_device->isChecked()) arguments.append("--erase");
  if (hardware_check->isChecked()) arguments.append("--hardware_check");
  return arguments;
}

QString MainWindow::run_process(QPlainTextEdit &target_plain_text_edit, const QStringList &process_arguments, const QString &type = "stderr") {
  QString output = "";
  
  QString minipro_path = system_sett_path->text().trimmed();
  if (minipro_path.isEmpty() || minipro_path.contains("Enter minipro path")) {
      minipro_path = "minipro";
  }

  QString process_arguments_string = "";
  auto *process = new QProcess();
  for (auto const &each : process_arguments) {
    process_arguments_string += each + " ";
  }
  target_plain_text_edit.appendPlainText("[Input]: minipro " + process_arguments_string);
  process->start(minipro_path, process_arguments);

  if (!process->waitForStarted()) {
    output += "Start Error";
  }
  if (!process->waitForFinished()) {
    output += "Finished Error";
  }
  if (type == "stderr") {
    output += process->readAllStandardError();
    static QRegularExpression re(R"(Serial code:.*\n([\s\S]*))");
    if (QRegularExpressionMatch match = re.match(output); match.hasMatch()) {
      output = match.captured(1).trimmed();
    }
    target_plain_text_edit.appendPlainText("[Output]: " + output.replace("\u001B[K", ""));
  } else if (type == "stdout") {
    output += process->readAllStandardOutput();
    static QRegularExpression re(R"(Serial code:.*\n([\s\S]*))");
    if (QRegularExpressionMatch match = re.match(output); match.hasMatch()) {
      output = match.captured(1).trimmed().replace("\u001B[K", "");
    }
  }
  target_plain_text_edit.ensureCursorVisible();
  return output;
}

void MainWindow::check_for_minipro() {

  QString minipro_path = system_sett_path->text().trimmed();
  if (minipro_path.isEmpty() || minipro_path.contains("Enter minipro path")) {
      minipro_path = "null";
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  QStringList arguments;
  arguments << "--version";
  
  auto initial_check_error = run_process(*status_view, arguments);
  
  if (initial_check_error.length() > 0 && initial_check_error.contains("minipro version")) {
    QApplication::restoreOverrideCursor();
    static QRegularExpression re("minipro version.*\\n");
    QRegularExpressionMatch match = re.match(initial_check_error);
    if (match.hasMatch()) {
      window->setWindowTitle(match.captured(0).trimmed());
      minipro_found = true;
      check_for_programmer();
      get_devices();
      set_preferences("minipro_path", minipro_path);
    } else {
      combobox_programmer->setDisabled(true);
    }
  } else {
    QApplication::restoreOverrideCursor();
    // Se fallisce, resetta lo stato
    QMessageBox msgBox;
    msgBox.setText("minipro CLI not found or Invalid Path");
    msgBox.exec();
    minipro_found = false;
    programmer_found = false;
    disable_buttons();
    window->setWindowTitle("minipro CLI not found or Invalid Path");
  }
}

void MainWindow::select_output_path_dialog(){
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("Binary (*.bin)"));
  dialog.setOption(QFileDialog::DontResolveSymlinks);
  dialog.setOption(QFileDialog::DontUseNativeDialog, true);
  dialog.setDirectory(QDir::homePath());
  if (dialog.exec()){
    save_file_name = dialog.selectedFiles().first();
    output_path->setText(save_file_name);
  }
}

void MainWindow::select_path_dialog(){
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setOption(QFileDialog::DontResolveSymlinks);
  dialog.setOption(QFileDialog::DontUseNativeDialog, true);
  dialog.setDirectory(QDir::rootPath());
  dialog.exec();
  QStringList files = dialog.selectedFiles();
  if (!files.isEmpty()) {
    system_sett_path->setText(files.first());
  }else{
    QMessageBox msgBox;
    msgBox.setText("minipro not selected");
    msgBox.exec();
  }
}

void MainWindow::check_for_programmer() {
  if (minipro_found) {
    QStringList arguments;
    arguments << "--presence_check";
    combobox_programmer->clear();
    static QRegularExpression re("(?<=: ).*$");
    QRegularExpressionMatch match = re.match(run_process(*status_view, arguments).trimmed());
    if (match.hasMatch()) {
      programmer = match.captured(0);
      programmer_found = true;
      combobox_programmer->addItem(programmer);
      enable_buttons();
    } else {
      combobox_programmer->setPlaceholderText("No programmer found");
      programmer_found = false;
      disable_buttons();
    }
  }
}

void MainWindow::set_preferences(const QString &preference_key, QString &preference_value){
  QSettings settings("minipro", "minipro-gui");
  settings.setValue(preference_key, preference_value);
}

QString MainWindow::get_preferences(const QString &preference_key){
  QSettings settings("minipro", "minipro-gui");
  QString val = settings.value(preference_key, QString("null")).toString();
  return val;
}


void MainWindow::disable_buttons() {
  combobox_device->setDisabled(true);
  button_run_command->setDisabled(true);
  button_read->setDisabled(true);
  button_write->setDisabled(true);
  button_update->setDisabled(true);
}

void MainWindow::enable_buttons() {
  combobox_device->setDisabled(false);
  button_run_command->setDisabled(false);
  button_read->setDisabled(false);
  button_write->setDisabled(false);
  button_update->setDisabled(false);
}

void MainWindow::get_devices() {
  if (programmer_found) {
    QStringList arguments;
    arguments << "-l";

    devices_list.clear();
    devices_list << run_process(*status_view, arguments, "stdout").split("\n", Qt::SkipEmptyParts);
    devices_list.sort();
    devices_list.removeDuplicates();

    combobox_device->addItems(devices_list);
  }
}

void MainWindow::select_device(const QString &selected_device) {
  if (minipro_found && selected_device != "") {
    device = selected_device;
    QStringList arguments;
    arguments << "-d" << device;

    static QRegularExpression re;
    re.setPatternOptions(QRegularExpression::MultilineOption);
    re.setPattern("Name:([\\s\\S]*)($)");
    QRegularExpressionMatch filter = re.match(run_process(*status_view, arguments));

    QRegularExpressionMatch match;
    if (filter.hasMatch()) {
      re.setPattern("(?<=Name: ).*$");
      match = re.match(filter.captured(0));
      device_name->setText(match.captured(0));
      re.setPattern("(?<=Memory: ).*$");
      match = re.match(filter.captured(0));
      device_memory->setText(match.captured(0));
      re.setPattern("(?<=Package: ).*$");
      match = re.match(filter.captured(0));
      device_package->setText(match.captured(0));
      re.setPattern("(?<=Protocol: ).*$");
      match = re.match(filter.captured(0));
      device_protocol->setText(match.captured(0));
      re.setPattern("(?<=Read buffer size: ).*$");
      match = re.match(filter.captured(0));
      device_readbuffer->setText(match.captured(0));
      re.setPattern("(?<=Write buffer size: ).*$");
      match = re.match(filter.captured(0));
      device_writebuffer->setText(match.captured(0));
      build_default_hex_output();
    }
  }
}


void MainWindow::run_command() {
  if (minipro_found) {
    QStringList arguments;
    arguments << "-p" << device;
    arguments.append(parse_checkboxes());
    run_process(*status_view, arguments);
  }
}

void MainWindow::read_device() {
  save_file_name = output_path->text();
  if(!save_file_name.isEmpty()){
    QStringList arguments;
    arguments << "-p" << device << "-r" << save_file_name;
    run_async_process(arguments);
  }
}

void MainWindow::read_device_output(int code, QProcess::ExitStatus) {
  hexViewModel.clearHexTable();
  if (code != 1) {
    build_formatted_hex_output();
  } else {
    build_default_hex_output();
  }
}

void MainWindow::write_device() {
  QString fileName = QFileDialog::getOpenFileName(this);
  if (fileName != "") {
    QStringList arguments;
    arguments << "-p" << device << "-w" << fileName;
    run_async_process(arguments);
  }
}

void MainWindow::update_firmware() {
  QString fileName = QFileDialog::getOpenFileName(this);
  if (fileName != "") {
    QStringList arguments;
    arguments << "-p" << device << "-F" << fileName;
    run_async_process(arguments);
  }
}

void MainWindow::build_default_hex_output() {
  hexViewModel.clearHexTable();
  format_hex_table_columns();
}

void MainWindow::build_formatted_hex_output() {
  try {
    QFile temp_file(save_file_name);
    if(temp_file.open(QFile::ReadOnly)){
      QString temp_file_content = QString::fromUtf8(temp_file.readAll().toHex());
      temp_file.close();

      hexViewModel.buildHexTable(temp_file_content);
      format_hex_table_columns();
    }
  }
  catch (const std::exception &e) {
    status_view->appendPlainText("\n[Error]: " + static_cast<QString>(e.what()));
  }
}

void MainWindow::format_hex_table_columns() const {
  hexTableView->horizontalHeader()->setStretchLastSection(true);
  for (int n = 0; n <= hexViewModel.columnCount(); n++) {
    hexTableView->resizeColumnToContents(n);
  }
  for (int n = 0; n <= hexViewModel.rowCount(); n++) {
    hexTableView->resizeRowToContents(n);
  }
}
