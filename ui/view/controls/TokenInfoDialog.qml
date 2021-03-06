import QtQuick 2.11
import QtQuick.Controls 2.4

import QtQuick.Layouts 1.11
import Beam.Wallet 1.0
import "."

Dialog {
    id: dialog
    modal: true
        
    property alias token:                   viewModel.token
    property alias defaultAddressType:      viewModel.defaultPermanent
    property alias ignoreStoredVouchers:    viewModel.ignoreStoredVouchers
    
    TokenInfoItem {
        id:     viewModel
    }

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    
    parent: Overlay.overlay
    padding: 0

    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape

    onClosed: {
        
    }

    onOpened: {
        forceActiveFocus();
    }

    background: Rectangle {
        radius: 10
        color: Style.background_popup
        anchors.fill: parent
    }

    contentItem: ColumnLayout {
        GridLayout {
            Layout.margins:         30
            rowSpacing:             20
            columnSpacing:          16
            columns:                2
        
            RowLayout {
                Layout.columnSpan: 2
                SFText {
                    Layout.fillWidth:   true
                    horizontalAlignment:Text.AlignHCenter
                    leftPadding:        30
                    font.pixelSize:     18
                    font.styleName:     "Bold"
                    font.weight:        Font.Bold
                    color:              Style.content_main
                    //% "Transaction address"
                    text:               qsTrId("address-info-title")
                }
            }

            // Amount
            SFText {
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Amount"
                text:                   qsTrId("general-amount") + ":"
                visible:                viewModel.amount.length
            }
            
            SFLabel {
                Layout.fillWidth:       true
                copyMenuEnabled:        true
                wrapMode:               Text.Wrap
                font.pixelSize:         14
                color:                  Style.content_main
                text:                   viewModel.amount
                visible:                viewModel.amount.length
            }
        
            // Address expiration
            SFText {
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Address expiration"
                text:                   qsTrId("address-expiration") + ":"
                visible:                viewModel.hasAddressType
            }
            
            SFText {
                Layout.fillWidth:       true
                wrapMode:               Text.Wrap
                font.pixelSize:         14
                text:                   viewModel.isPermanent ? 
                                        //% "Permanent" 
                                        qsTrId("address-info-dialog-permanent")
                                        //% "One time"
                                        : qsTrId("token-info-dialog-one-time")
                color:                  Style.content_main
                visible:                viewModel.hasAddressType
            }

            // Transaction type:
            SFText {
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Transaction type"
                text:                   qsTrId("address-info-transaction-type") + ":"
                visible:                viewModel.transactionType.length
            }
            
            SFText {
                Layout.fillWidth:       true
                wrapMode:               Text.Wrap
                font.pixelSize:         14
                color:                  Style.content_main
                text:                   viewModel.transactionType
                verticalAlignment:      Text.AlignBottom
                visible:                viewModel.transactionType.length
            }

            // Address type
            SFText {
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Address type"
                text:                   qsTrId("address-info-type") + ":"
                visible:                viewModel.tokenType.length
            }
            
            SFText {
                Layout.fillWidth:       true
                wrapMode:               Text.Wrap
                font.pixelSize:         14
                text:                   viewModel.tokenType
                color:                  Style.content_main
                visible:                viewModel.tokenType.length
            }
            
            // Address
            SFText {
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "SBBS Address"
                text:                   qsTrId("address-info-sbbs-address") + ":"
                visible:                viewModel.address.length && !viewModel.isMaxPrivacy
            }
            
            SFLabel {
                Layout.fillWidth:       true
                copyMenuEnabled:        true
                wrapMode:               Text.Wrap
                font.pixelSize:         14
                color:                  Style.content_main
                text:                   viewModel.address
                onCopyText:             BeamGlobals.copyToClipboard(text)
                visible:                viewModel.address.length && !viewModel.isMaxPrivacy
            }

            // Identity
            SFText {
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Identity"
                text:                   qsTrId("general-identity") + ":"
                visible:                viewModel.identity.length
            }
            
            SFLabel {
                Layout.fillWidth:       true
                copyMenuEnabled:        true
                wrapMode:               Text.Wrap
                font.pixelSize:         14
                color:                  Style.content_main
                text:                   viewModel.identity
                onCopyText:             BeamGlobals.copyToClipboard(text)
                visible:                viewModel.identity.length
            }
            
            
            // Address
            SFText {
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Address"
                text:                   qsTrId("address-info-address") + ":"
                visible:                viewModel.token != viewModel.address
            }
            
            ScrollView {
                Layout.fillWidth:             true
                Layout.preferredWidth:        578
                Layout.maximumHeight:         200
                clip:                         true
                ScrollBar.horizontal.policy:  ScrollBar.AlwaysOff
                ScrollBar.vertical.policy:    ScrollBar.AsNeeded
                visible:                      viewModel.token != viewModel.address
                SFLabel {
                    //Layout.fillWidth:       true
                    //Layout.preferredWidth:  578
                    width:                    578
                    copyMenuEnabled:          true
                    wrapMode:                 Text.Wrap
                    font.pixelSize:           14
                    color:                    Style.content_main
                    text:                     viewModel.token
                    onCopyText:               BeamGlobals.copyToClipboard(text)
                }
            }
        }
        Row {
            id:                     buttonsLayout
            Layout.fillHeight:      true
            Layout.alignment:       Qt.AlignHCenter
            Layout.leftMargin:      30
            Layout.rightMargin:     30
            Layout.bottomMargin:    30
            spacing:                20
        
            CustomButton {
                icon.source:    "qrc:/assets/icon-cancel-16.svg"
                //% "Close"
                text:           qsTrId("general-close")
                onClicked: {
                    dialog.close()
                }
            }
            
            PrimaryButton {
                icon.source:        "qrc:/assets/icon-copy-blue.svg"
                palette.buttonText: Style.content_opposite
                icon.color:         Style.content_opposite
                palette.button:     Style.accent_incoming
                //% "Copy address"
                text:               qsTrId("address-info-copy-address")
                onClicked: {
                    BeamGlobals.copyToClipboard(viewModel.token);
                }
            }
        }
    }
}