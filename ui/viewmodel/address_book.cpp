#include "address_book.h"
#include "ui_helpers.h"

using namespace std;
using namespace beam;
using namespace beamui;

PeerAddressItem::PeerAddressItem()
	: m_walletID{}
	, m_name{}
	, m_category{}
{

}

PeerAddressItem::PeerAddressItem(const beam::WalletAddress& address)
    : m_walletID{beamui::toString(address.m_walletID)}
    , m_name{QString::fromStdString(address.m_label)}
    , m_category(QString::fromStdString(address.m_category))
{

}

QString PeerAddressItem::getWalletID() const
{
    return m_walletID;
}

void PeerAddressItem::setWalletID(const QString& value)
{
	m_walletID = value;
}

QString PeerAddressItem::getName() const
{
    return m_name;
}

void PeerAddressItem::setName(const QString& value)
{
    m_name = value;
}

QString PeerAddressItem::getCategory() const
{
    return m_category;
}

void PeerAddressItem::setCategory(const QString& value)
{
    m_category = value;
}

OwnAddressItem::OwnAddressItem()
	: PeerAddressItem{}
	, m_createDate{}
	, m_expirationDate{}
{

}

OwnAddressItem::OwnAddressItem(const beam::WalletAddress& address) 
    : PeerAddressItem{ address }
    , m_createDate{toString(address.m_createTime)}
    , m_expirationDate{toString(address.m_createTime + address.m_duration)}
{

}

QString OwnAddressItem::getExpirationDate() const
{
    return m_expirationDate;
}

QString OwnAddressItem::getCreateDate() const
{
    return m_createDate;
}


AddressBookViewModel::AddressBookViewModel(WalletModel& model)
    : m_model{model}
{
    connect(&m_model, SIGNAL(onStatus(const WalletStatus&)),
        SLOT(onStatus(const WalletStatus&)));

    connect(&m_model, SIGNAL(onAdrresses(bool, const std::vector<beam::WalletAddress>&)),
        SLOT(onAdrresses(bool, const std::vector<beam::WalletAddress>&)));

	connect(&m_model, SIGNAL(onGeneratedNewWalletID(const beam::WalletID&)),
		SLOT(onGeneratedNewWalletID(const beam::WalletID&)));

    /*for (int i = 0; i < 100; ++i)
    {
        beam::WalletAddress a = {};
        a.m_own = false;
        a.m_label = "Peer address " + to_string(i);
        ECC::Hash::Processor() << a.m_label.c_str() >> a.m_walletID;
        a.m_category = "work";
        m_peerAddresses.push_back(new PeerAddressItem(a));
    }
    for (int i = 0; i < 20; ++i)
    {
        beam::WalletAddress a = {};
        a.m_own = true;
        a.m_label = "My address " + to_string(i);
        ECC::Hash::Processor() << a.m_label.c_str() >> a.m_walletID;
        a.m_createTime = beam::getTimestamp();
        a.m_duration = 1000000;
        a.m_category = "work";
        m_ownAddresses.push_back(new OwnAddressItem(a));
    }*/
}

QVariant AddressBookViewModel::getPeerAddresses() const
{
    return QVariant::fromValue(m_peerAddresses);
}

QVariant AddressBookViewModel::getOwnAddresses() const
{
    return QVariant::fromValue(m_ownAddresses);
}

void AddressBookViewModel::setNewPeerAddress(QVariant addr)
{

}

void AddressBookViewModel::setNewOwnAddress(QVariant addr)
{

}

QVariant AddressBookViewModel::getNewPeerAddress()
{
	return QVariant::fromValue(&m_newPeerAddress);
}

QVariant AddressBookViewModel::getNewOwnAddress()
{
	return QVariant::fromValue(&m_newOwnAddress);
}

void AddressBookViewModel::generateNewEmptyAddress()
{
	m_ownAddresses = {};
	m_peerAddresses = {};

	if (m_model.async)
	{
		m_model.async->generateNewWalletID();
	}
}

void AddressBookViewModel::createNewAddress()
{
    WalletAddress a = {};
    a.m_own = false;
    a.m_label = "My address " + to_string(chrono::system_clock::now().time_since_epoch().count());
    ECC::Hash::Processor() << a.m_label.c_str() >> a.m_walletID;
    a.m_createTime = beam::getTimestamp();
    a.m_duration = 1000000;
    a.m_category = "work";

    if (m_model.async)
    {
        m_model.async->createNewAddress(std::move(a));
        m_model.async->getAddresses(true);
        m_model.async->getAddresses(false);
    }
}

void AddressBookViewModel::createNewPeerAddress()
{
	WalletAddress peerAddress{};
	peerAddress.m_own = false;
	peerAddress.m_label = m_newPeerAddress.getName().toStdString();
	peerAddress.m_createTime = beam::getTimestamp();
	peerAddress.m_category = m_newPeerAddress.getCategory().toStdString();

	if (m_model.async)
	{
		m_model.async->createNewAddress(std::move(peerAddress));
		m_model.async->getAddresses(false);
	}
}

void AddressBookViewModel::createNewOwnAddress()
{
	WalletAddress ownAddress{};

	ownAddress.m_own = true;
	ownAddress.m_label = m_newOwnAddress.getName().toStdString();
	ownAddress.m_createTime = beam::getTimestamp();
	//ownAddress.m_duration = m_newOwnAddress.getExpirationDate
	ownAddress.m_category = m_newOwnAddress.getCategory().toStdString();

	if (m_model.async)
	{
		m_model.async->createNewAddress(std::move(ownAddress));
		m_model.async->getAddresses(true);
	}
}

Q_INVOKABLE QVariant AddressBookViewModel::getPeerAddress(int index) const
{
    return QVariant::fromValue(m_peerAddresses[index]);
}

void AddressBookViewModel::onStatus(const WalletStatus&)
{
    if (m_model.async)
    {
        m_model.async->getAddresses(true);
        m_model.async->getAddresses(false);
    }
}

void AddressBookViewModel::onAdrresses(bool own, const std::vector<WalletAddress>& addresses)
{
    if (own)
    {
        m_ownAddresses.clear();

        for (const auto& addr : addresses)
        {
            m_ownAddresses.push_back(new OwnAddressItem(addr));
        }
    }
    else
    {
        m_peerAddresses.clear();

        for (const auto& addr : addresses)
        {
            m_peerAddresses.push_back(new PeerAddressItem(addr));
        }
    }

    emit addressesChanged();
}

void AddressBookViewModel::onGeneratedNewWalletID(const beam::WalletID& walletID)
{
	m_newOwnAddress.setWalletID(toString(walletID));

	emit newAddressChanged();
}