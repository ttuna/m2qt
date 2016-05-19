TEMPLATE = subdirs

SUBDIRS += \
    ../m2qt \
    ../M2QtApp \
    ../M2QtTest

M2QtApp.depends = m2qt
