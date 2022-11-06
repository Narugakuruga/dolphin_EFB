// Copyright 2020 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <QDockWidget>

#include "Common/CommonTypes.h"

#include "Common/Debug/CodeTrace.h"

class QCloseEvent;
class QPushButton;
class QCheckBox;
class QLineEdit;
class QLabel;
class QComboBox;
class QTableWidget;
class QSpinBox;

struct TraceResults
{
  TraceOutput trace_output;
  HitType type;
  std::set<std::string> regs;
};

class TraceWidget : public QDockWidget
{
  Q_OBJECT
public:
  explicit TraceWidget(QWidget* parent = nullptr);
  ~TraceWidget();

  void UpdateFont();
  void AutoStep(CodeTrace::AutoStop option, const std::string reg);
  void UpdateBreakpoints();
  void resizeEvent(QResizeEvent* event) override;

signals:
  void ShowCode(u32 address);
  void ShowMemory(u32 address);

protected:
  void closeEvent(QCloseEvent*) override;

private:
  void CreateWidgets();
  void ConnectWidgets();
  void ClearAll();
  u32 GetVerbosity() const;
  void OnRecordTrace(bool checked);
  void LogCreated(std::optional<QString> target_register = std::nullopt);
  std::vector<TraceResults> CodePath(u32 start, u32 end, size_t results_limit);
  std::vector<TraceResults> MakeTraceFromLog();
  void DisplayTrace();
  void InfoDisp();

  void OnContextMenu();
  void OnSetColor(QColor* text_color);
  QTableWidget* m_output_table;
  QLineEdit* m_trace_target;
  QLineEdit* m_range_start;
  QLineEdit* m_range_end;
  QComboBox* m_record_stop_addr;
  QCheckBox* m_backtrace;
  QCheckBox* m_show_values;
  QCheckBox* m_filter_overwrite;
  QCheckBox* m_filter_move;
  QCheckBox* m_filter_loadstore;
  QCheckBox* m_filter_pointer;
  QCheckBox* m_filter_passive;
  QCheckBox* m_filter_active;
  QCheckBox* m_clear_on_loop;
  QCheckBox* m_change_range;
  QPushButton* m_reprocess;
  QLabel* m_record_limit_label;
  QLabel* m_results_limit_label;
  QSpinBox* m_record_limit_input;
  QSpinBox* m_results_limit_input;
  QPushButton* m_record_trace;

  QColor m_tracked_color = Qt::blue;
  QColor m_overwritten_color = Qt::red;
  QColor m_value_color = Qt::darkGreen;

  std::vector<TraceOutput> m_code_trace;
  size_t m_record_limit = 150000;
  std::vector<QString> m_error_msg;
  int m_font_vspace = 0;
  bool m_recording = false;
};
