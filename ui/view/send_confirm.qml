import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import Beam.Wallet 1.0
import "controls"
import "./utils.js" as Utils

ConfirmationDialog {
    onVisibleChanged: {
        if (!this.visible) {
            this.destroy();
        }
    }

    id: sendViewConfirm
    parent: Overlay.overlay

    property var    onAcceptedCallback: undefined
    property alias  addressText:        addressLabel.text
    property alias  typeText:           typeLabel.text
    property bool   swapMode:           false
    property int    currency:           Currency.CurrBeam
    property string amount:             "0"
    property string fee:                "0"
    property string secondCurrencyRate:         "0"
    property string secondCurrencyLabel:        ""
    readonly property bool      showSecondCurrency: secondCurrencyLabel != ""
    readonly property string    currencyLabel:  BeamGlobals.getCurrencyLabel(sendViewConfirm.currency)
    readonly property string    feeLabel:       !sendViewConfirm.swapMode
                                                //% "Fee"
                                                ? (qsTrId("send-regular-fee") + ":")
                                                : (sendViewConfirm.currency == Currency.CurrBeam
                                                    //% "BEAM Transaction fee"
                                                    ? (qsTrId("beam-transaction-fee") + ":")
                                                    //% "%1 Transaction fee rate"
                                                    : qsTrId("general-fee-rate").arg(currencyLabel))
    property Item   defaultFocusItem:   BeamGlobals.needPasswordToSpend() ? requirePasswordInput : cancelButton

    okButtonText: sendViewConfirm.swapMode ?
                    //% "Swap"
                    qsTrId("general-swap"):
                    //% "Send"
                    qsTrId("general-send")
    okButtonColor:           Style.accent_outgoing
    okButtonIconSource:      "qrc:/assets/icon-send-blue.svg"
    okButtonEnable:          BeamGlobals.needPasswordToSpend() ? requirePasswordInput.text.length : true
    cancelButtonIconSource:  "qrc:/assets/icon-cancel-white.svg"

    function confirmationHandler() {
        if (BeamGlobals.needPasswordToSpend()) {
            if (requirePasswordInput.text.length == 0) {
                requirePasswordInput.forceActiveFocus(Qt.TabFocusReason);
                return;
            }
            if (!BeamGlobals.isPasswordValid(requirePasswordInput.text)) {
                requirePasswordInput.forceActiveFocus(Qt.TabFocusReason);
                requirePasswordInput.selectAll();
                requirePasswordError.text = qsTrId("general-pwd-invalid");
                return;
            }
        }

        accepted();
        close();
    }

    function openHandler() {
        defaultFocusItem.forceActiveFocus(Qt.TabFocusReason);
    }

    function passworInputEnter() {
        okButton.forceActiveFocus(Qt.TabFocusReason);
        okButton.clicked();
    }

    function getFeeInSecondCurrency(feeValue) {
        return Utils.formatFeeToSecondCurrency(
            feeValue,
            sendViewConfirm.secondCurrencyRate,
            sendViewConfirm.secondCurrencyLabel)
    }

    function getAmountInSecondCurrency() {
        return Utils.formatAmountToSecondCurrency(
            sendViewConfirm.amount,
            sendViewConfirm.secondCurrencyRate,
            sendViewConfirm.secondCurrencyLabel);
    }

    onAccepted: {
        onAcceptedCallback();
    }
    topPadding: 30
    contentItem:
    ColumnLayout {
        spacing:      30

        SFText {
            id: title
            Layout.alignment:   Qt.AlignHCenter
            font.pixelSize:     18
            font.styleName:     "Bold";
            font.weight:        Font.Bold
            color:              Style.content_main
            text:               sendViewConfirm.swapMode ?
                                //% "Confirm atomic swap"
                                qsTrId("send-swap-confirmation-title") :
                                //% "Confirm transaction details"
                                qsTrId("send-confirmation-title")
        }

        GridLayout {
            Layout.fillWidth:       true
            Layout.fillHeight:      true
            columnSpacing:          14
            rowSpacing:             14
            columns:                2

            //
            // Recipient/Address
            //
            SFText {
                Layout.fillWidth:       false
                Layout.fillHeight:      true
                Layout.minimumHeight:   16
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Recipient"
                text:                   qsTrId("send-confirmation-recipient-label") + ":"
                verticalAlignment:      Text.AlignTop
            }

            property bool showMultiline: addressLabel.text.length < 68

            SFLabel {
                id:                     addressLabel
                Layout.fillWidth:       true
                Layout.maximumWidth:    290
                Layout.minimumHeight:   16
                wrapMode:               parent.showMultiline ? Text.Wrap : Text.NoWrap
                maximumLineCount:       2
                elide:                  parent.showMultiline ? Text.ElideNone : Text.ElideMiddle
                font.pixelSize:         14
                color:                  Style.content_main
                copyMenuEnabled:        true
                onCopyText:             BeamGlobals.copyToClipboard(text)
            }

            //
            // Transaction type
            //
            SFText {
                Layout.fillWidth:       true
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Type"
                text:                   qsTrId("send-confirmation-type-label") + ":"
                verticalAlignment:      Text.AlignTop
                visible:                typeLabel.text.length > 0
            }

            SFText {
                id:                     typeLabel
                Layout.fillWidth:       true
                Layout.maximumWidth:    290
                Layout.minimumHeight:   16
                wrapMode:               Text.Wrap
                maximumLineCount:       2
                font.pixelSize:         14
                color:                  Style.content_main
                visible:                text.length > 0
            }

            //
            // Amount
            //
            SFText {
                Layout.fillWidth:       true
                Layout.alignment:       Qt.AlignTop
                Layout.topMargin:       8
                font.pixelSize:         14
                color: Style.content_disabled
                //% "Amount"
                text: qsTrId("general-amount") + ":"
                verticalAlignment: Text.AlignTop
            }

            ColumnLayout {
                Layout.fillWidth:       true
                SFText {
                    id:                     amountLabel
                    font.pixelSize:         24
                    color:                  Style.accent_outgoing
                    text: [
                            Utils.uiStringToLocale(sendViewConfirm.amount),
                            sendViewConfirm.currencyLabel
                        ].join(" ")
                }
                SFText {
                    id:             secondCurrencyAmountLabel
                    visible:        sendViewConfirm.showSecondCurrency
                    font.pixelSize: 14
                    color:          Style.content_disabled
                    text:           getAmountInSecondCurrency()
                }
            }

            //
            // Fee
            //
            SFText {
                Layout.fillWidth:       true
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                color:                  Style.content_disabled
                text:                   sendViewConfirm.feeLabel
                verticalAlignment:      Text.AlignTop
            }

            ColumnLayout {
                Layout.fillWidth:   true
                SFText {
                    id:                     feeLabel
                    font.pixelSize:         14
                    color:                  Style.content_main
                    text: [
                            Utils.uiStringToLocale(sendViewConfirm.fee),
                            BeamGlobals.getFeeRateLabel(sendViewConfirm.currency)
                        ].join(" ")
                }
                SFText {
                    id:                 secondCurrencyFeeLabel
                    visible:            sendViewConfirm.showSecondCurrency
                    font.pixelSize:     14
                    color:              Style.content_disabled
                    text:               getFeeInSecondCurrency(parseInt(sendViewConfirm.fee, 10))
                }
            }

            //
            // Password confirmation
            //
            SFText {
                id:                     requirePasswordLabel
                visible:                BeamGlobals.needPasswordToSpend()
                Layout.columnSpan:      2
                Layout.topMargin:       16
                horizontalAlignment:    Text.AlignHCenter
                Layout.fillWidth:       true
                Layout.minimumHeight:   16
                font.pixelSize:         14
                color:                  Style.content_main
                //% "To broadcast your transaction please enter your password"
                text:                   qsTrId("send-confirmation-pwd-require-message")
            }

            SFTextInput {
                id:                     requirePasswordInput
                visible:                BeamGlobals.needPasswordToSpend()
                Layout.columnSpan:      2
                Layout.fillWidth:       true
                focus:                  true
                activeFocusOnTab:       true
                font.pixelSize:         14
                color:                  Style.content_main
                echoMode:               TextInput.Password
                onAccepted:             passworInputEnter()
                onTextChanged:          if (requirePasswordError.text.length > 0) requirePasswordError.text = ""
            }

            SFText {
                Layout.fillWidth:       true
                Layout.columnSpan:      2
                id:                     requirePasswordError
                visible:                BeamGlobals.needPasswordToSpend()
                color:                  Style.validator_error
                font.pixelSize:         14
            }

            //
            // Wait online message
            //
            SFText {
                Layout.columnSpan:      2
                Layout.topMargin:       0//15
                Layout.bottomMargin:    15
                horizontalAlignment:    Text.AlignHCenter
                Layout.fillWidth:       sendViewConfirm.swapMode
                Layout.maximumHeight:   60
                Layout.maximumWidth:    sendViewConfirm.swapMode ? parent.width : 400
                Layout.minimumHeight:   16
                font.pixelSize:         14
                color:                  Style.content_disabled
                wrapMode:               Text.WordWrap
                text: sendViewConfirm.swapMode ?
                    //% "Keep your wallet online. The swap normally takes about 1 hour to complete."
                    qsTrId("send-swap-sconfirmation-online-time") + (sendViewConfirm.currency !== Currency.CurrBeam ?
                        //% " Once the offer is accepted by the other side, the %1 transaction fee will be charged even if the offer is cancelled."
                        qsTrId("send-swap-fee-warning").arg(sendViewConfirm.currencyLabel)
                        : "")
                    :
                    //% "For the transaction to complete, the recipient must get online within the next 12 hours and you should get online within 2 hours afterwards."
                    qsTrId("send-confirmation-pwd-text-online-time")
            }
        }
    }
}