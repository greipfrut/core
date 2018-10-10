/**************************************************************************************
Copyright 2015 Applied Research Associates, Inc.
Licensed under the Apache License, Version 2.0 (the "License"); you may not use
this file except in compliance with the License. You may obtain a copy of the License
at:
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
**************************************************************************************/

#include <biogears/cdm/patient/actions/SEOverride.h>

#include <biogears/cdm/properties/SEScalarPressure.h>
#include <biogears/cdm/properties/SEScalarTemperature.h>


namespace biogears {
SEOverride::SEOverride()
  : SEPatientAction()
{
  m_OverrideSwitch = CDM::enumOnOff::Off;
  m_OverrideValid = CDM::enumOnOff::Off;
  m_PressureOR = nullptr;
  m_CoreTemperatureOR = nullptr;
}

SEOverride::~SEOverride()
{
  Clear();
}

void SEOverride::Clear()
{
  SEPatientAction::Clear();
  m_OverrideSwitch = CDM::enumOnOff::Off;
  m_OverrideValid = CDM::enumOnOff::Off;
  SAFE_DELETE(m_PressureOR);
  SAFE_DELETE(m_CoreTemperatureOR);
}

bool SEOverride::IsValid() const
{
  if (!HasOverrideSwitch()) {
    Error("Override must have state.");
    return false;
  }
  if ((!HasMAPOverride() && !HasCoreTemperatureOverride()) && GetOverrideSwitch() == CDM::enumOnOff::On) {
    Error("Override must have a parameter");
    return false;
  }
  return true;
}

bool SEOverride::IsActive() const
{
  if (!HasOverrideSwitch() || !HasOverrideValidity())
    return false;
  return GetOverrideSwitch() == CDM::enumOnOff::On;
}

bool SEOverride::Load(const CDM::OverrideData& in)
{
  SEPatientAction::Clear();
  SetOverrideSwitch(in.State());
  SetOverrideValidity(in.Valid());
  if (in.MeanArterialPressureOverride().present())
    GetMAPOverride().Load(in.MeanArterialPressureOverride().get());
  else
    GetMAPOverride().Invalidate();
  if (in.CoreTemperatureOverride().present())
    GetCoreTemperatureOverride().Load(in.CoreTemperatureOverride().get());
  else
    GetCoreTemperatureOverride().Invalidate();
  //SEPatientAction::Load(in);
  //return true;
  return IsValid();
}

CDM::OverrideData* SEOverride::Unload() const
{
  CDM::OverrideData* data(new CDM::OverrideData());
  Unload(*data);
  return data;
}

void SEOverride::Unload(CDM::OverrideData& data) const
{
  SEPatientAction::Unload(data);
  if (HasOverrideSwitch())
    data.State(m_OverrideSwitch);
  if (HasOverrideValidity())
    data.Valid(m_OverrideValid);
  if (HasMAPOverride())
    data.MeanArterialPressureOverride(std::unique_ptr<CDM::ScalarPressureData>(m_PressureOR->Unload()));
  if (HasCoreTemperatureOverride())
    data.CoreTemperatureOverride(std::unique_ptr<CDM::ScalarTemperatureData>(m_CoreTemperatureOR->Unload()));
}

CDM::enumOnOff::value SEOverride::GetOverrideSwitch() const
{
  return m_OverrideSwitch;
}
void SEOverride::SetOverrideSwitch(CDM::enumOnOff::value state)
{
  m_OverrideSwitch = state;
}
bool SEOverride::HasOverrideSwitch() const
{
  return (m_OverrideSwitch == CDM::enumOnOff::Off) ? true : (m_OverrideSwitch == CDM::enumOnOff::On) ? true : false;
}
void SEOverride::InvalidateOverrideSwitch()
{
  m_OverrideSwitch = (CDM::enumOnOff::Off);
}
CDM::enumOnOff::value SEOverride::GetOverrideValidity() const
{
  return m_OverrideValid;
}
void SEOverride::SetOverrideValidity(CDM::enumOnOff::value valid)
{
  m_OverrideValid = valid;
}
bool SEOverride::HasOverrideValidity() const
{
  return (m_OverrideValid == CDM::enumOnOff::Off) ? true : (m_OverrideValid == CDM::enumOnOff::On) ? true : false;
}
void SEOverride::InvalidateOverrideValidity()
{
  m_OverrideValid = (CDM::enumOnOff::Off);
}
bool SEOverride::IsOverrideValid()
{
  return (m_OverrideValid == CDM::enumOnOff::On) ? true : false;
}

// Cardiovascular Overrides //
bool SEOverride::HasMAPOverride() const
{
  return m_PressureOR == nullptr ? false : m_PressureOR->IsValid();
}
SEScalarPressure& SEOverride::GetMAPOverride()
{
  if (m_PressureOR == nullptr)
    m_PressureOR = new SEScalarPressure();
  return *m_PressureOR;
}
double SEOverride::GetMAPOverride(const PressureUnit& unit) const
{
  if (m_PressureOR == nullptr)
    return SEScalar::dNaN();
  return m_PressureOR->GetValue(unit);
}

bool SEOverride::HasCardiovascularOverride() const
{
  return HasMAPOverride() ? true : false;
}

// Energy Overrides //
bool SEOverride::HasCoreTemperatureOverride() const
{
  return m_CoreTemperatureOR == nullptr ? false : m_CoreTemperatureOR->IsValid();
}
SEScalarTemperature& SEOverride::GetCoreTemperatureOverride()
{
  if (m_CoreTemperatureOR == nullptr)
    m_CoreTemperatureOR = new SEScalarTemperature();
  return *m_CoreTemperatureOR;
}
double SEOverride::GetCoreTemperatureOverride(const TemperatureUnit& unit) const
{
  if (m_CoreTemperatureOR == nullptr)
    return SEScalar::dNaN();
  return m_CoreTemperatureOR->GetValue(unit);
}

bool SEOverride::HasEnergyOverride() const
{
  return HasCoreTemperatureOverride() ? true : false;
}

void SEOverride::ToString(std::ostream& str) const
{
  str << "Patient Action : Override Parameter";
  if (HasComment())
    str << "\n\tComment: " << m_Comment;

  str << "\n\tState: ";
  HasOverrideSwitch() ? str << GetOverrideSwitch() : str << "Not Set";
  str << "\n\tValid: ";
  HasOverrideValidity() ? str << GetOverrideValidity() : str << "Not Set";
  if (GetOverrideValidity() == CDM::enumOnOff::Off) {
    str << ("\n\tOverride has turned validity off. Outputs no longer resemble validated parameters.");
  }
  if (HasMAPOverride())
  {
    str << "\n\tPressure: ";
    HasMAPOverride() ? str << *m_PressureOR : str << "Not Set";
    str << std::flush;
  }
  if (HasCoreTemperatureOverride())
  {
    str << "\n\tCore Temperature: ";
    HasCoreTemperatureOverride() ? str << *m_CoreTemperatureOR : str << "Not Set";
    str << std::flush;
  } 
}
}
