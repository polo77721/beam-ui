// Copyright 2018 The Beam Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "receive_swap_view.h"
#include "ui_helpers.h"
#include "model/app_model.h"
#include "wallet/swaps/common.h"
#include "wallet/swaps/swap_transaction.h"
#include "wallet/bitcoin/bitcoin_side.h"
#include "wallet/litecoin/litecoin_side.h"
#include "wallet/qtum/qtum_side.h"
#include <QClipboard>
#include "qml_globals.h"

namespace {
    enum {
        OfferExpires12h = 0,
        OfferExpires6h  = 1
    };

    uint16_t GetHourCount(int offerExpires)
    {
        switch (offerExpires)
        {
        case OfferExpires12h:
            return 12;
        case OfferExpires6h:
            return 6;
        default:
        {
            assert(false && "Unexpected value!");
            return 0;
        }
        }
    }

    beam::Height GetBlockCount(int offerExpires)
    {
        return GetHourCount(offerExpires) * 60;
    }

    // TODO: remove after tests
    beam::Height GetTestBlockCount(int offerExpires)
    {
        switch (offerExpires)
        {
        case OfferExpires12h:
            return 20;
        case OfferExpires6h:
            return 10;
        default:
        {
            assert(false && "Unexpected value!");
            return 0;
        }
        }
    }
}

ReceiveSwapViewModel::ReceiveSwapViewModel()
    : _amountToReceiveGrothes(0)
    , _amountSentGrothes(0)
    , _receiveFeeGrothes(0)
    , _sentFeeGrothes(0)
    , _receiveCurrency(Currency::CurrBeam)
    , _sentCurrency(Currency::CurrBtc)
    , _offerExpires(OfferExpires12h)
    , _walletModel(*AppModel::getInstance().getWallet())
    , _txParameters(beam::wallet::CreateSwapParameters()
        .SetParameter(beam::wallet::TxParameterID::AtomicSwapCoin, beam::wallet::AtomicSwapCoin::Bitcoin)
        .SetParameter(beam::wallet::TxParameterID::AtomicSwapIsBeamSide, true)
        .SetParameter(beam::wallet::TxParameterID::IsInitiator, true))
{
    connect(&_walletModel, &WalletModel::generatedNewAddress, this, &ReceiveSwapViewModel::onGeneratedNewAddress);
    connect(&_walletModel, SIGNAL(newAddressFailed()), this, SIGNAL(newAddressFailed()));
    connect(&_walletModel, &WalletModel::stateIDChanged, this, &ReceiveSwapViewModel::updateTransactionToken);

    generateNewAddress();

    _walletModel.getAsync()->getWalletStatus();
    updateTransactionToken();
}

void ReceiveSwapViewModel::onGeneratedNewAddress(const beam::wallet::WalletAddress& addr)
{
    _receiverAddress = addr;
    emit receiverAddressChanged();
    updateTransactionToken();
}

QString ReceiveSwapViewModel::getAmountToReceive() const
{
    return beamui::amount2ui(_amountToReceiveGrothes);
}

void ReceiveSwapViewModel::setAmountToReceive(QString value)
{
    auto amout = beamui::ui2amount(value);
    if (amout != _amountToReceiveGrothes)
    {
        _amountToReceiveGrothes = amout;
        emit amountToReceiveChanged();
        updateTransactionToken();
    }
}

QString ReceiveSwapViewModel::getAmountSent() const
{
    return beamui::amount2ui(_amountSentGrothes);
}

unsigned int ReceiveSwapViewModel::getReceiveFee() const
{
    return _receiveFeeGrothes;
}

void ReceiveSwapViewModel::setAmountSent(QString value)
{
    auto amout = beamui::ui2amount(value);
    if (amout != _amountSentGrothes)
    {
        _amountSentGrothes = amout;
        emit amountSentChanged();
        updateTransactionToken();
    }
}

unsigned int ReceiveSwapViewModel::getSentFee() const
{
    return _sentFeeGrothes;
}

void ReceiveSwapViewModel::setSentFee(unsigned int value)
{
    if (value != _sentFeeGrothes)
    {
        _sentFeeGrothes = value;
        emit sentFeeChanged();
    }
}

Currency ReceiveSwapViewModel::getReceiveCurrency() const
{
    return _receiveCurrency;
}

void ReceiveSwapViewModel::setReceiveCurrency(Currency value)
{
    assert(value > Currency::CurrStart && value < Currency::CurrEnd);

    if (value != _receiveCurrency)
    {
        _receiveCurrency = value;
        emit receiveCurrencyChanged();
    }
}

Currency ReceiveSwapViewModel::getSentCurrency() const
{
    return _sentCurrency;
}

void ReceiveSwapViewModel::setSentCurrency(Currency value)
{
    assert(value > Currency::CurrStart && value < Currency::CurrEnd);

    if (value != _sentCurrency)
    {
        _sentCurrency = value;
        emit sentCurrencyChanged();
        updateTransactionToken();
    }
}

void ReceiveSwapViewModel::setReceiveFee(unsigned int value)
{
    if (value != _receiveFeeGrothes)
    {
        _receiveFeeGrothes = value;
        emit receiveFeeChanged();
    }
}

int ReceiveSwapViewModel::getOfferExpires() const
{
    return _offerExpires;
}

void ReceiveSwapViewModel::setOfferExpires(int value)
{
    if (value != _offerExpires)
    {
        _offerExpires = value;
        emit offerExpiresChanged();
        updateTransactionToken();
    }
}

QString ReceiveSwapViewModel::getReceiverAddress() const
{
    return beamui::toString(_receiverAddress.m_walletID);
}

void ReceiveSwapViewModel::generateNewAddress()
{
    _receiverAddress = {};
    emit receiverAddressChanged();

    setAddressComment("");
    _walletModel.getAsync()->generateNewAddress();
}

void ReceiveSwapViewModel::setTransactionToken(const QString& value)
{
    if (_token != value)
    {
        _token = value;
        emit transactionTokenChanged();
    }
}

QString ReceiveSwapViewModel::getTransactionToken() const
{
    return _token;
}

QString ReceiveSwapViewModel::getAddressComment() const
{
    return _addressComment;
}

void ReceiveSwapViewModel::setAddressComment(const QString& value)
{
    auto trimmed = value.trimmed();
    if (_addressComment != trimmed)
    {
        _addressComment = trimmed;
        emit addressCommentChanged();
        emit commentValidChanged();
    }
}

bool ReceiveSwapViewModel::getCommentValid() const
{
    return !_walletModel.isAddressWithCommentExist(_addressComment.toStdString());
}

bool ReceiveSwapViewModel::isEnough() const
{
    if (_amountSentGrothes == 0)
        return true;

    switch (_sentCurrency)
    {
    case Currency::CurrBeam:
    {
        auto total = _amountSentGrothes + _sentFeeGrothes;
        return _walletModel.getAvailable() >= total;
    }
    case Currency::CurrBtc:
    {
        // TODO sentFee is fee rate. should be corrected
        // TODO:double
        auto total = static_cast<double>(_amountSentGrothes + _sentFeeGrothes) / beam::wallet::UnitsPerCoin(beam::wallet::AtomicSwapCoin::Bitcoin);
        return AppModel::getInstance().getBitcoinClient()->getAvailable() > total;
    }
    case Currency::CurrLtc:
    {
        // TODO:double
        auto total = static_cast<double>(_amountSentGrothes + _sentFeeGrothes) / beam::wallet::UnitsPerCoin(beam::wallet::AtomicSwapCoin::Litecoin);
        return AppModel::getInstance().getLitecoinClient()->getAvailable() > total;
    }
    case Currency::CurrQtum:
    {
        // TODO:double
        auto total = static_cast<double>(_amountSentGrothes + _sentFeeGrothes) / beam::wallet::UnitsPerCoin(beam::wallet::AtomicSwapCoin::Qtum);
        return AppModel::getInstance().getQtumClient()->getAvailable() > total;
    }
    default:
    {
        assert(false);
        return true;
    }
    }
}

bool ReceiveSwapViewModel::isGreatThanFee() const
{
    if (_amountSentGrothes == 0)
        return true;

    switch (_sentCurrency)
    {
    case Currency::CurrBeam:
    {
        const auto total = _amountSentGrothes + _sentFeeGrothes;
        return total > QMLGlobals::minFeeBeam();
    }
    case Currency::CurrBtc:
    {
        return beam::wallet::BitcoinSide::CheckAmount(_amountSentGrothes, _sentFeeGrothes);
    }
    case Currency::CurrLtc:
    {
        return beam::wallet::LitecoinSide::CheckAmount(_amountSentGrothes, _sentFeeGrothes);
    }
    case Currency::CurrQtum:
    {
        return beam::wallet::QtumSide::CheckAmount(_amountSentGrothes, _sentFeeGrothes);
    }
    default:
    {
        assert(false);
        return true;
    }
    }
}

void ReceiveSwapViewModel::saveAddress()
{
    using namespace beam::wallet;

    if (getCommentValid()) {
        _receiverAddress.m_label = _addressComment.toStdString();
        _receiverAddress.m_duration = GetHourCount(_offerExpires) * WalletAddress::AddressExpiration1h;
        _walletModel.getAsync()->saveAddress(_receiverAddress, true);
    }
}

void ReceiveSwapViewModel::startListen()
{
    using namespace beam::wallet;

    bool isBeamSide = (_sentCurrency == Currency::CurrBeam);
    auto beamFee = isBeamSide ? _sentFeeGrothes : _receiveFeeGrothes;
    auto swapFee = isBeamSide ? _receiveFeeGrothes : _sentFeeGrothes;
    auto txParameters = beam::wallet::TxParameters(_txParameters);

    txParameters.DeleteParameter(TxParameterID::PeerID);
    txParameters.SetParameter(TxParameterID::IsInitiator, !*_txParameters.GetParameter<bool>(TxParameterID::IsInitiator));
    txParameters.SetParameter(TxParameterID::MyID, *_txParameters.GetParameter<beam::wallet::WalletID>(beam::wallet::TxParameterID::PeerID));
    txParameters.SetParameter(TxParameterID::Fee, beam::Amount(beamFee));
    txParameters.SetParameter(TxParameterID::Fee, beam::Amount(swapFee), isBeamSide ? SubTxIndex::REDEEM_TX : SubTxIndex::LOCK_TX);
    txParameters.SetParameter(TxParameterID::AtomicSwapIsBeamSide, isBeamSide);
    txParameters.SetParameter(TxParameterID::IsSender, isBeamSide);
    if (getCommentValid())
    {
        std::string localComment = _addressComment.toStdString();
        txParameters.SetParameter(TxParameterID::Message, beam::ByteBuffer(localComment.begin(), localComment.end()));
    }

    _walletModel.getAsync()->startTransaction(std::move(txParameters));
}

void ReceiveSwapViewModel::publishToken()
{
    auto packedTxParams = _txParameters.Pack();
    
    auto txId = _txParameters.GetTxID();
    auto publisherId = _txParameters.GetParameter<beam::wallet::WalletID>(beam::wallet::TxParameterID::PeerID);
    if (publisherId && txId)
    {
        beam::wallet::SwapOffer offer(*txId);
        offer.m_txId = *txId;
        offer.m_publisherId = *publisherId;
        offer.m_status = beam::wallet::SwapOfferStatus::Pending;
        offer.SetTxParameters(packedTxParams);
        
        _walletModel.getAsync()->publishSwapOffer(offer);
    }
}

namespace
{
    beam::wallet::AtomicSwapCoin convertCurrencyToSwapCoin(Currency currency)
    {
        switch (currency)
        {
        case Currency::CurrBtc:
            return beam::wallet::AtomicSwapCoin::Bitcoin;
        case Currency::CurrLtc:
            return beam::wallet::AtomicSwapCoin::Litecoin;
        case Currency::CurrQtum:
            return beam::wallet::AtomicSwapCoin::Qtum;
        default:
            return beam::wallet::AtomicSwapCoin::Unknown;
        }
    }
}

void ReceiveSwapViewModel::updateTransactionToken()
{
    emit enoughChanged();
    emit lessThanFeeChanged();
    _txParameters.SetParameter(beam::wallet::TxParameterID::MinHeight, _walletModel.getCurrentHeight());
    // TODO: uncomment after tests
    // _txParameters.SetParameter(beam::wallet::TxParameterID::PeerResponseTime, GetBlockCount(_offerExpires));
    // TODO: remove after tests
    _txParameters.SetParameter(beam::wallet::TxParameterID::PeerResponseTime, GetTestBlockCount(_offerExpires));

    // All parameters sets as if we were on the recipient side (mirrored)
    bool isBeamSide = (_receiveCurrency == Currency::CurrBeam);
    auto swapCoin   = convertCurrencyToSwapCoin(isBeamSide ? _sentCurrency : _receiveCurrency);
    auto beamAmount = isBeamSide ? _amountToReceiveGrothes : _amountSentGrothes;
    auto swapAmount = isBeamSide ? _amountSentGrothes : _amountToReceiveGrothes;

    _txParameters.SetParameter(beam::wallet::TxParameterID::AtomicSwapIsBeamSide, isBeamSide);
    _txParameters.SetParameter(beam::wallet::TxParameterID::Amount, beamAmount);
    _txParameters.SetParameter(beam::wallet::TxParameterID::AtomicSwapCoin, swapCoin);
    _txParameters.SetParameter(beam::wallet::TxParameterID::AtomicSwapAmount, swapAmount);
    _txParameters.SetParameter(beam::wallet::TxParameterID::PeerID, _receiverAddress.m_walletID);
    _txParameters.SetParameter(beam::wallet::TxParameterID::IsSender, isBeamSide);

    setTransactionToken(QString::fromStdString(std::to_string(_txParameters)));
}
