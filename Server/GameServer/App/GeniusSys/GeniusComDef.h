/** 
 *  @brief 天赋基本属性配置
 *
 */

#pragma  once 

enum  UpdateEvent
{
    Skill_Update =1,
    Genius_Update,
};

enum  RemoveEvent
{
    Skill_Remove=1,
    Genius_Remove,
};

#define  BASEGENIUSNUM      1000

namespace  NameGenius
{

    struct  TypeKey
    {
        string   key;
    };

    struct  TypeValue
    {
        long     Value;
    };

    struct  Traits
    {
        typedef TypeKey  KeyType;
        typedef long    ValueType;
    };

    struct  Property
    {
        Traits::KeyType       Key;
        Traits::ValueType     Value;

        Property( Traits::KeyType k,Traits::ValueType v):Key(k),Value(v)
        {

        }
    };

    template<class Key,class Value,
             class Table = std::map<Key,Value> >
    class  ProperyTable
    {

        typedef   Key   KeyType;
        typedef   Value ValueType;
        typedef   Table TableType;

    public:

        void    AddProperty( Key k , Value v )
        {
            m_MapTable[ k ] = v;
        }
        void    RemoveProperty( Key k )
        {
            m_MapTable.erase( k );
        }

        ValueType  GetVar(Key k)
        {
            Table::iterator  it = m_MapTable.find( k );
            if ( it != m_MapTable.end() )
            {
                return it->second;
            }
            return 0;
        }
            
        void    ModifyVar(Key k,Value v)
        {
            Table::iterator  it = m_MapTable.find( k );
            if ( it != m_MapTable.end() )
            {
                it->second = v;
            }
        }

        void    Clear()
        {
            m_MapTable.clear();
        }

    private:
        Table   m_MapTable;

    };

}
