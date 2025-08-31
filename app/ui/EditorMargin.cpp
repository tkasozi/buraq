//
// Created by talik on 5/28/2025.
//
#include "EditorMargin.h"
#include <QHBoxLayout> // Required for QHBoxLayout
#include <QPainter>
#include <QTextBlock>

EditorMargin::EditorMargin(QWidget* windowPtr, QWidget* parent) : CommonWidget(parent), windowPtr(windowPtr)
{
    setFixedWidth(82); // The EditorMargin itself has a fixed width

    // No need to explicitly call `setLayout(main_layout)` if `main_layout`
    // was constructed with `this` as its parent: `new QHBoxLayout(this)`.
    // This will be the main horizontal layout for EditorMargin
    const auto main_layout = new QHBoxLayout(this); // Passing 'this' sets the layout on EditorMargin
    main_layout->setSpacing(0);
    main_layout->setContentsMargins(0, 0, 0, 0);

    // --- 2nd Widget: Line Numbers Area ---
    // This widget will be for line numbers
    // Ensure line_numbers is a QWidget. If it's a custom class deriving QWidget, that's fine.
    // If it's just a raw QWidget that you intend to draw on, that's also fine.

    // Layout for the line_numbers widget (e.g., if you add more items to it later)
    const auto line_number_internal_layout = new QVBoxLayout(line_numbers_widget.get()); // Set this layout on the line_numbers widget
    line_number_internal_layout->setSpacing(0);
    line_number_internal_layout->setContentsMargins(0, 0, 0, 0);

    line_numbers_widget = std::make_unique<LineNumberAreaWidget>(this);
    line_numbers_widget->setObjectName("EditorMargin");

    // Then, add a stretchable space after it
    line_number_internal_layout->addStretch(1);

    // Add the line_numbers widget to the main horizontal layout
    // Example stretch factor, adjust as needed (e.g., 0 for default, 1 for some stretch)
    main_layout->addWidget(line_numbers_widget.get());

    // --- 1st Widget: Action Area (contains CodeRunner) ---
    // This widget will hold the codeRunner
    const auto action_widget = new QWidget(this); // Parent to EditorMargin

    const auto action_layout = new QVBoxLayout(action_widget); // Layout for the action_widget
    action_layout->setSpacing(0);
    action_layout->setContentsMargins(0, 0, 0, 0);

    codeRunner = std::make_unique<CodeRunner>(windowPtr); // Assuming CodeRunner is a QWidget or QWidget-derived
    action_layout->addWidget(codeRunner.get());

    // Then, add a stretchable space after it
    action_layout->addStretch(1);

    // Add the action_widget to the main horizontal layout
    main_layout->addWidget(action_widget); // Default stretch factor

    EditorMargin::setupSignals();
}

// Call this method whenever the editor's state changes
// For example, when text is edited, or the view is scrolled.
void EditorMargin::updateState(const buraq::EditorState& newState) const
{
    if (line_numbers_widget)
    {
        line_numbers_widget->updateEditorState(newState);
    }
}

void EditorMargin::onEditorScrolled()
{
}

void EditorMargin::updateMarginWidth(const buraq::EditorState& state) const
{
    line_numbers_widget->updateEditorState(state);
}

void EditorMargin::setupSignals() const
{
    // add impl
}
