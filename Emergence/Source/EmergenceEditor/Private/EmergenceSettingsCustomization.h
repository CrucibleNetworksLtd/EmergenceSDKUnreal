// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"

class FEmergenceSettingsCustomization: public IDetailCustomization
{
public:
    // IDetailCustomization interface
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
    //

    static TSharedRef< IDetailCustomization > MakeInstance();
};