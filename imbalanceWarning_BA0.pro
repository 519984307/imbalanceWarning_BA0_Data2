#-------------------------------------------------
#
# Project created by QtCreator 2021-02-22T11:03:59
#
#-------------------------------------------------

QT       += core gui sql network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imbalanceWarning_BA0_211122
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    json/json_parser.cc \
    json/json_scanner.cc \
    json/json_scanner.cpp \
    json/parser.cpp \
    json/parserrunnable.cpp \
    json/qobjecthelper.cpp \
    json/serializer.cpp \
    json/serializerrunnable.cpp \
    TestInput_Cursor/formhanzitable.cpp \
    TestInput_Cursor/formsymbolpad.cpp \
    TestInput_Cursor/HanziManager.cpp \
    TestInput_Cursor/formhanziselector.cpp \
    TestInput_Cursor/formnumpad.cpp \
    TestInput_Cursor/padbase.cpp \
    TestInput_Cursor/keyboard.cpp \
    TestInput_Cursor/dialogtooltip.cpp \
    TestInput_Cursor/inputmethod.cpp \
    passwordpanel.cpp \
    settingswindow.cpp \
    confirmationbox.cpp \
    querydialog.cpp \
    sqlquery.cpp \
    fisrequest.cpp \
    mydatetime.cpp \
    logdelete.cpp \
    updatetime.cpp \
    rfidmanage.cpp \
    rfid_sick.cpp

HEADERS  += mainwindow.h \
    json/FlexLexer.h \
    json/json_parser.hh \
    json/json_scanner.h \
    json/location.hh \
    json/parser_p.h \
    json/parser.h \
    json/parserrunnable.h \
    json/position.hh \
    json/qjson_debug.h \
    json/qjson_export.h \
    json/qobjecthelper.h \
    json/serializer.h \
    json/serializerrunnable.h \
    json/stack.hh \
    TestInput_Cursor/formhanzitable.h \
    TestInput_Cursor/keyboard.h \
    TestInput_Cursor/dialogtooltip.h \
    TestInput_Cursor/ChineseTranslator.h \
    TestInput_Cursor/formhanziselector.h \
    TestInput_Cursor/formnumpad.h \
    TestInput_Cursor/HanziManager.h \
    TestInput_Cursor/formsymbolpad.h \
    TestInput_Cursor/inputmethod.h \
    TestInput_Cursor/padbase.h \
    passwordpanel.h \
    settingswindow.h \
    confirmationbox.h \
    querydialog.h \
    sqlquery.h \
    fisrequest.h \
    mydatetime.h \
    logdelete.h \
    updatetime.h \
    rfidmanage.h \
    rfid_sick.h

FORMS    += mainwindow.ui \
    TestInput_Cursor/formsymbolpad.ui \
    TestInput_Cursor/formnumpad.ui \
    TestInput_Cursor/formhanziselector.ui \
    TestInput_Cursor/keyboard.ui \
    TestInput_Cursor/dialogtooltip.ui \
    TestInput_Cursor/formhanzitable.ui \
    passwordpanel.ui \
    settingswindow.ui \
    confirmationbox.ui \
    querydialog.ui

RESOURCES += \
    icon/icon.qrc \
    TestInput_Cursor/keypad.qrc
