#include "SelectionDialog.h"
#include "ui_selectionConsole.h"
#include "dataModel/Gene.h"

SelectionDialog::SelectionDialog(QSharedPointer<DataProxy> dataProxy,
                                 QWidget *parent,
                                 Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_ui(new Ui::SelectionDialog())
    , m_includeAmbiguous(false)
    , m_caseSensitive(false)
    , m_regExpValid(false)
    , m_selectNonVisible(false)
    , m_dataProxy(dataProxy)
{
    Q_ASSERT(!m_dataProxy.isNull());

    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    m_ui->setupUi(this);

    // set default state
    slotCaseSensitive(false);
    slotIncludeAmbiguous(false);
    slotSelectNonVisible(false);
    move(parent->window()->mapToGlobal(parent->window()->rect().center())
         - mapToGlobal(rect().center()));

    // NOTE the connections are made in the UI file

    m_regExp.setPatternSyntax(QRegExp::WildcardUnix);
}

SelectionDialog::~SelectionDialog()
{
}

const SelectionDialog::GeneList &SelectionDialog::selectedGenes() const
{
    return m_selectedGeneList;
}

const SelectionDialog::GeneList SelectionDialog::selectGenes(QSharedPointer<DataProxy> dataProxy,
                                                             QWidget *parent)
{
    SelectionDialog dialog(dataProxy, parent);
    dialog.setWindowIcon(QIcon());

    if (dialog.exec() == QDialog::Accepted) {
        return dialog.selectedGenes();
    }

    return SelectionDialog::GeneList();
}

void SelectionDialog::accept()
{
    // early out, should "never" happen
    if (!m_regExp.isValid()) {
        reject();
        return;
    }

    // find all genes that match the regular expression
    m_selectedGeneList.clear();
    for (auto &gene : m_dataProxy->getGeneList()) {
        const QString name = gene->name();
        // filter for ambiguos genes and unselected
        // if the options are correct
        if ((!m_includeAmbiguous && gene->isAmbiguous())
            || (!m_selectNonVisible && !gene->selected())) {
            continue;
        }

        if (m_regExp.exactMatch(name)) {
            // at this point all included genes must be selected
            gene->selected(true);
            m_selectedGeneList.push_back(gene);
        }
    }

    // and propagate accept call
    QDialog::accept();
}

void SelectionDialog::slotValidateRegExp(const QString &pattern)
{
    m_regExp.setPattern(pattern);
    const bool regExpValid = m_regExp.isValid();
    if (regExpValid != m_regExpValid) {
        m_regExpValid = regExpValid;
    }
}

void SelectionDialog::slotIncludeAmbiguous(bool includeAmbiguous)
{
    m_includeAmbiguous = includeAmbiguous;
    if (m_includeAmbiguous != m_ui->checkAmbiguous->isChecked()) {
        m_ui->checkAmbiguous->setChecked(m_includeAmbiguous);
    }
}

void SelectionDialog::slotSelectNonVisible(bool selectNonVisible)
{
    m_selectNonVisible = selectNonVisible;
    if (m_selectNonVisible != m_ui->checkSelectNonVisible->isChecked()) {
        m_ui->checkSelectNonVisible->setChecked(m_selectNonVisible);
    }
}

void SelectionDialog::slotCaseSensitive(bool caseSensitive)
{
    // toggle case sensitive
    m_regExp.setCaseSensitivity((caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive));
    m_caseSensitive = caseSensitive;
    if (m_caseSensitive != m_ui->checkCaseSense->isChecked()) {
        m_ui->checkCaseSense->setChecked(m_caseSensitive);
    }
}

void SelectionDialog::slotEnableAcceptAction(bool enableAcceptAction)
{
    for (QAbstractButton *button : m_ui->buttonBox->buttons()) {
        const QDialogButtonBox::ButtonRole role = m_ui->buttonBox->buttonRole(button);
        if (role == QDialogButtonBox::AcceptRole || role == QDialogButtonBox::YesRole
            || role == QDialogButtonBox::ApplyRole) {
            button->setEnabled(enableAcceptAction);
        }
    }
}
