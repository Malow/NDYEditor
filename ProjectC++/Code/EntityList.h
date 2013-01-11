#pragma once

#include <string>

void LoadEntList( const std::string& fileName ) throw(...);
const std::string& GetEntName( unsigned int entIndex ) throw(...);
const std::string& GetEntModel( unsigned int entIndex ) throw(...);
unsigned int GetEntListSize();