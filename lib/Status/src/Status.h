#include <string>
#include <Firebase_ESP_Client.h>

class Status
{

private:
    std::string statusFileName;
    FirebaseJson json;
    void persist();
public:
    Status(std::string ss);    
    void setUpdated();
    bool isUpdated();
};