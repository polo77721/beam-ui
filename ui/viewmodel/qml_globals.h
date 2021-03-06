// Copyright 2019 The Beam Team
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
#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include "currencies.h"
#include "wallet/transactions/swaps/common.h"

class QMLGlobals : public QObject
{
    Q_OBJECT
public:
    QMLGlobals(QQmlEngine&);

    Q_INVOKABLE static void showMessage(const QString& message);
    Q_INVOKABLE static void copyToClipboard(const QString& text);
    Q_INVOKABLE QString version();
    Q_INVOKABLE static bool isAddress(const QString& text);
    Q_INVOKABLE static bool isTransactionToken(const QString& text);
    Q_INVOKABLE static bool isSwapToken(const QString& text);
    Q_INVOKABLE static bool isTAValid(const QString& text);
    Q_INVOKABLE static QString getLocaleName();
    Q_INVOKABLE static int  maxCommentLength();
    Q_INVOKABLE static bool needPasswordToSpend();
    Q_INVOKABLE static bool isPasswordValid(const QString& value);

    // Currency utils
    static bool isFeeOK(unsigned int fee, Currency currency, bool isShielded);
    static bool isSwapFeeOK(unsigned int amount, unsigned int fee, Currency currency);
    static int  getMinFeeOrRate(Currency currency);
    static beam::wallet::AtomicSwapCoin convertCurrencyToSwapCoin(Currency currency);
    static Currency convertSwapCoinToCurrency(beam::wallet::AtomicSwapCoin swapCoin);
    Q_INVOKABLE static QString calcTotalFee(Currency currency, unsigned int feeRate);
    Q_INVOKABLE static QString calcFeeInSecondCurrency(int fee, const QString& exchangeRate, const QString& secondCurrencyLabel);
    Q_INVOKABLE static QString calcAmountInSecondCurrency(const QString& amount, const QString& exchangeRate, const QString& secondCurrLabel);

    Q_INVOKABLE static unsigned int minFeeBeam(bool isShielded = false);
    
    Q_INVOKABLE static QString getCurrencyLabel(Currency);
    Q_INVOKABLE static QString getCurrencyName(Currency);
    Q_INVOKABLE static QString getFeeRateLabel(Currency);
    Q_INVOKABLE static QString getCurrencySubunitFromLabel(const QString& currLabel);
    
    Q_INVOKABLE static unsigned int getMinimalFee(Currency, bool isShielded);
    Q_INVOKABLE static unsigned int getRecommendedFee(Currency);
    Q_INVOKABLE static unsigned int getDefaultFee(Currency);

    // Swap & other currencies utils
    Q_INVOKABLE static bool canSwap();
    Q_INVOKABLE static bool haveSwapClient(Currency);

    Q_INVOKABLE static QString rawTxParametrsToTokenStr(const QVariant& variantTxParams);

    Q_INVOKABLE static bool canReceive(Currency currency);
    Q_INVOKABLE static QString divideWithPrecision8(const QString& dividend, const QString& divider);
    Q_INVOKABLE static QString multiplyWithPrecision8(const QString& first, const QString& second);
    Q_INVOKABLE static QString roundWithPrecision8(const QString& number);
private:
    QQmlEngine& _engine;
};
