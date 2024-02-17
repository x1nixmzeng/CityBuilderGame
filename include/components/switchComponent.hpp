#pragma once
#include "component.hpp"
#include "misc/cells.hpp"

#include <string>
#include <vector>

struct TriggerData {
    std::string value;
    CellPos cellOffset;
};

struct TriggerComponent : public Component<false> {

    std::string const name;
    CellPos const cell;
    std::vector<TriggerData> triggerData;

    inline TriggerComponent(const std::string& name_, const CellPos& cell_, const std::vector<TriggerData>&& triggerData_)
        : name(name_), cell(cell_), triggerData(triggerData_) {
    }

    inline TriggerComponent(const std::string& name_, const CellPos& cell_, const std::vector<TriggerData>& triggerData_)
        : name(name_), cell(cell_), triggerData(triggerData_) {
    }

    inline TriggerComponent(const std::string& name_, const CellPos& cell_, std::initializer_list<TriggerData> triggerData_ = {})
        : name(name_), cell(cell_), triggerData(triggerData_) {
    }
};
