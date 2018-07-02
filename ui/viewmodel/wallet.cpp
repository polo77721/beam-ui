#include "wallet.h"

using namespace beam;

TxObject::TxObject(const QString& dateVal
	, const QString& userVal
	, const QString& commentVal
	, const QString& amountVal
	, const QString& amountUsdVal
	, const QString& statusVal)
	: _date(dateVal)
	, _user(userVal)
	, _comment(commentVal)
	, _amount(amountVal)
	, _amountUsd(amountUsdVal)
	, _status(statusVal)
{

}

QString TxObject::date() const
{
	return _date;
}

QString TxObject::user() const
{
	return _user;
}

QString TxObject::comment() const
{
	return _comment;
}

QString TxObject::amount() const
{
	return _amount;
}

QString TxObject::amountUsd() const
{
	return _amountUsd;

}

QString TxObject::status() const
{
	return _status;
}

WalletViewModel::WalletViewModel(IKeyChain::Ptr keychain)
	: _model(keychain)
	, _available(0)
{
	_tx.append(new TxObject(
		"12 June 2018 | 3:46 PM"
		, "super_user"
		, "Beam is super cool, bla bla bla..."
		, "+0.63736 BEAM"
		, "726.4 USD"
		, "unspent"));
	_tx.append(new TxObject(
		"12 June 2018 | 3:46 PM"
		, "super_user"
		, "Beam is super cool, bla bla bla..."
		, "+0.63736 BEAM"
		, "726.4 USD"
		, "unspent"));
	_tx.append(new TxObject(
		"12 June 2018 | 3:46 PM"
		, "super_user"
		, "Beam is super cool, bla bla bla..."
		, "+0.63736 BEAM"
		, "726.4 USD"
		, "unspent"));
	_tx.append(new TxObject(
		"12 June 2018 | 3:46 PM"
		, "super_user"
		, "Beam is super cool, bla bla bla..."
		, "+0.63736 BEAM"
		, "726.4 USD"
		, "unspent"));
	_tx.append(new TxObject(
		"12 June 2018 | 3:46 PM"
		, "super_user"
		, "Beam is super cool, bla bla bla..."
		, "+0.63736 BEAM"
		, "726.4 USD"
		, "unspent"));
	_tx.append(new TxObject(
		"12 June 2018 | 3:46 PM"
		, "super_user"
		, "Beam is super cool, bla bla bla..."
		, "+0.63736 BEAM"
		, "726.4 USD"
		, "unspent"));

	connect(&_model, SIGNAL(onStatus(const beam::Amount&)), SLOT(onStatus(const beam::Amount&)));

	_model.start();
}

void WalletViewModel::onStatus(const beam::Amount& amount)
{
	if (_available != amount)
	{
		_available = amount;

		emit availableChanged();
	}
}

QString WalletViewModel::available() const
{
	return QString::number(static_cast<float>(_available) / Rules::Coin) + " BEAM";
}

QVariant WalletViewModel::tx() const
{
	return QVariant::fromValue(_tx);
}