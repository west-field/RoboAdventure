#include "ItemFactory.h"
#include "Item/ItemCoin.h"

namespace
{
    const wchar_t* const kModelName = L"Data/Model/Star.mv1";
}

ItemFactory::ItemFactory() : m_isGetCoin()
{
    m_handleMap[ItemType::Coin1] = MV1LoadModel(kModelName);
}

ItemFactory::~ItemFactory()
{
    for (auto& handle : m_handleMap)
    {
        MV1DeleteModel(handle.second);
    }
}

void ItemFactory::Update()
{
    GetCoin();
    //なくなったアイテムを消す
    auto rmIt = std::remove_if(m_items.begin(), m_items.end(),
        [](const std::shared_ptr<ItemBase>& item) {
            return !item->IsExist();
        });
    m_items.erase(rmIt, m_items.end());

    //更新
    for (auto& item : m_items)
    {
        if (item->IsExist())
        {
            item->Update();
        }
    }
}

void ItemFactory::Draw()
{
    for (auto& item : m_items)
    {
        if (item->IsExist())
        {
            item->Draw();
        }
    }
}

void ItemFactory::Create(ItemType type, const VECTOR pos)
{
    switch (type)
    {
    case ItemType::Coin1:
        m_items.push_back(std::make_shared<ItemStar>(pos, m_handleMap[ItemType::Coin1],1));
        break;
    case ItemType::Coin2:
        m_items.push_back(std::make_shared<ItemStar>(pos, m_handleMap[ItemType::Coin1],2));
        break;
    case ItemType::Coin3:
        m_items.push_back(std::make_shared<ItemStar>(pos, m_handleMap[ItemType::Coin1],3));
        break;
    default:
        break;
    }
}

std::list<std::shared_ptr<ItemBase>> ItemFactory::GetItems()
{
    return m_items;
}

void ItemFactory::IsGetCoin(bool data[3])
{
    data[0] = m_isGetCoin[0];
    data[1] = m_isGetCoin[1];
    data[2] = m_isGetCoin[2];
}

void ItemFactory::GetCoin()
{
    for (auto& item : m_items)
    {
        if (item->IsExist())    continue;

        if (item->GetCoin() == 0)    continue;

        m_isGetCoin[(item->GetCoin() - 1)] = true;

    }
}
