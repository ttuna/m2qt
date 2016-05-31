TEMPLATE = subdirs

SUBDIRS += \
    ../m2qt \
    ../M2QtApp \
    ../M2QtTest \
    ../DOS-Starter-Demo

M2QtApp.depends = m2qt
DOS-Starter-Demo.depends = m2qt
