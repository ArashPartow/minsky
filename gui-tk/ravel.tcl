# Ravel specific functionality

proc ravelContextItems {} {
            .wiring.context add command -label "Export as CSV" -command exportItemAsCSV
            global sortOrder
            set sortOrder [minsky.canvas.item.sortOrder]
            .wiring.context add cascade -label "Set Next Aggregation" -menu .wiring.context.nextAggregationMenu
            .wiring.context add cascade -label "Axis properties" -menu .wiring.context.axisMenu
            if [llength [info commands minsky.canvas.item.lockGroup]] {
                .wiring.context add command -label "Lock specific handles" -command lockSpecificHandles
                .wiring.context add command -label "Unlock" -command {
                    minsky.canvas.item.leaveLockGroup; canvas.requestRedraw
                }
            }
}

proc setupPickDimMenu {} {
    global dimLabelPicked
    if {![winfo exists .wiring.context.pick]} {
        toplevel .wiring.context.pick
        wm title .wiring.context.pick "Pick any two dimensions"
        frame .wiring.context.pick.select
        scrollbar .wiring.context.pick.select.vscroll -orient vertical -command {
            .wiring.context.pick.select.lb yview}
        listbox .wiring.context.pick.select.lb -listvariable dimLabelPicked \
            -selectmode extended -selectforeground blue \
            -width 35 \
            -yscrollcommand {.wiring.context.pick.select.vscroll set} 
        pack .wiring.context.pick.select.lb -fill both  -expand y -side left
        pack .wiring.context.pick.select.vscroll -fill y -expand y -side left
        pack .wiring.context.pick.select
        buttonBar .wiring.context.pick {
            set pick {}
            foreach i [.wiring.context.pick.select.lb curselection] {
                lappend pick [lindex $dimLabelPicked $i]
            }
			minsky.canvas.item.setDimLabelsPicked [lindex $pick 0] [lindex $pick 1]
            reset
        }
        button .wiring.context.pick.buttonBar.clear -text "Clear" -command {
            .wiring.context.pick.select.lb selection clear 0 end}
        pack .wiring.context.pick.buttonBar.clear -side left
    } else {
        deiconify .wiring.context.pick
    }
        
    set dimLabelPicked [minsky.canvas.item.dimLabels]
    wm transient .wiring.context.pick
    wm geometry .wiring.context.pick +[winfo pointerx .]+[winfo pointery .]
    ensureWindowVisible .wiring.context.pick
    grab set .wiring.context.pick
}    

proc lockAttributes {attribute numRows column} {
    grid [label .wiring.context.lockHandles.grid.name$column -text $attribute] -row 0 -column $column
    for {set i 0} {$i<[expr $numRows-1]} {incr i} {
        grid [checkbutton .wiring.context.lockHandles.grid.name${column}_$i] -row [expr $i+1] -column $column
        .wiring.context.lockHandles.grid.name${column}_$i select
    }
}

proc lockSpecificHandles {} {
    global currentLockHandles

    if {![llength [info commands minsky.canvas.item.lockGroup.allLockHandles]]} {
        minsky.canvas.lockRavelsInSelection
        # reinitialise the canvas item commands
        getItemAt [minsky.canvas.item.x] [minsky.canvas.item.y]
        if {![llength [info commands minsky.canvas.item.lockGroup.allLockHandles]]} return
    }    
    if {[winfo exists .wiring.context.lockHandles]} {destroy .wiring.context.lockHandles}
    toplevel .wiring.context.lockHandles
    frame .wiring.context.lockHandles.grid
    pack .wiring.context.lockHandles.grid
    set ravel 0
    set maxRows 0
    foreach r [minsky.canvas.item.lockGroup.ravelNames] {
        grid [label .wiring.context.lockHandles.grid.name$ravel -text $r] -row 0 -column $ravel
        set handles [minsky.canvas.item.lockGroup.handleNames $ravel]
        for {set row 0} {$row<[llength $handles]} {incr row} {
            grid [ttk::combobox .wiring.context.lockHandles.grid.handle${ravel}_$row -state readony -values [concat "-" $handles]] -row [expr $row+1] -column $ravel
            if {$ravel==0} {
                .wiring.context.lockHandles.grid.handle${ravel}_$row set [lindex $handles $row]
            } else {
                if {$row<[llength $handles] && [lsearch $handles [.wiring.context.lockHandles.grid.handle0_$row get]]>-1} {
                    .wiring.context.lockHandles.grid.handle${ravel}_$row set [.wiring.context.lockHandles.grid.handle0_$row get]
                }
            }
        }
        if {[llength $handles] > $maxRows} {set maxRows [llength $handles]}
        incr ravel
    }
    incr maxRows
    
    foreach attribute {"Slicer" "Orientation" "Calipers" "SortOrder"} {
        lockAttributes $attribute $maxRows $ravel
        incr ravel
    }

    buttonBar .wiring.context.lockHandles {
        set lh {}
        global currentLockHandles
        foreach i [array names currentLockHandles] {
            if $currentLockHandles($i) {
                lappend lh $i
            }
        }
        minsky.canvas.item.lockGroup.setLockHandles $lh
    }
}

set aggTypes {{"Σ" "sum"} {"Π" "prod"} {"av" "av"} {"σ" "stddev"} {"min" "min"} {"max" "max"}}
menu .wiring.context.nextAggregationMenu
foreach m $aggTypes {
    .wiring.context.nextAggregationMenu add command -label [lindex $m 0] -command "minsky.canvas.item.nextReduction [lindex $m 1]"
}

menu .wiring.context.axisAggregationMenu
foreach m $aggTypes {
    .wiring.context.axisAggregationMenu add command -label [lindex $m 0] -command "minsky.canvas.item.handleSetReduction \[minsky.canvas.item.selectedHandle\] [lindex $m 1]"
}

menu .wiring.context.axisMenu 
.wiring.context.axisMenu add command -label "Description" -command {
    textEntryPopup .wiring.context.axisMenu.desc [minsky.canvas.item.description] {
        minsky.canvas.item.setDescription [.wiring.context.axisMenu.desc.entry get]
    }
}
.wiring.context.axisMenu add command -label "Dimension" -command setDimension
.wiring.context.axisMenu add command -label "Toggle Calipers" -command {
    minsky.canvas.item.toggleDisplayFilterCaliper
    minsky.canvas.item.broadcastStateToLockGroup
    reset
}
.wiring.context.axisMenu add cascade -label "Set Aggregation" -menu .wiring.context.axisAggregationMenu
menu .wiring.context.axisMenu.sort -postcommand populateSortOptions
.wiring.context.axisMenu add cascade -label "Sort" -menu .wiring.context.axisMenu.sort
set sortOrder none

proc populateSortOptions {} {
    set orders {none forward reverse}
    .wiring.context.axisMenu.sort delete 0 end
    foreach order $orders {
        .wiring.context.axisMenu.sort add radiobutton -label $order -command {
            minsky.canvas.item.setSortOrder $sortOrder
            minsky.canvas.item.broadcastStateToLockGroup
            reset
        } -value "$order" -variable sortOrder
    }
    .wiring.context.axisMenu.sort add command -label "forward by value" -command {minsky.canvas.item.sortByValue "forward"; reset}
    .wiring.context.axisMenu.sort add command -label "reverse by value" -command {minsky.canvas.item.sortByValue "reverse"; reset}
}

.wiring.context.axisMenu add command -label "Pick Slices" -command setupPickMenu


proc setDimension {} {
    if {![winfo exists .wiring.context.axisMenu.dim]} {
        toplevel .wiring.context.axisMenu.dim
        wm title .wiring.context.axisMenu.dim "Dimension axis"
        frame .wiring.context.axisMenu.dim.type
        label .wiring.context.axisMenu.dim.type.label -text "type"
        ttk::combobox .wiring.context.axisMenu.dim.type.value -values {string value time} -state readonly -textvariable axisType
        bind .wiring.context.axisMenu.dim.type.value <<ComboboxSelected>> {
            minsky.value.csvDialog.spec.horizontalDimension.type [.wiring.context.axisMenu.dim.type.value get]
            dimFormatPopdown .wiring.context.axisMenu.dim.units.value [.wiring.context.axisMenu.dim.type.value get] {}
        }
        pack .wiring.context.axisMenu.dim.type.label .wiring.context.axisMenu.dim.type.value -side left
        frame .wiring.context.axisMenu.dim.units
        label .wiring.context.axisMenu.dim.units.label -text "units/format"
        tooltip .wiring.context.axisMenu.dim.units.label \
     "Value type: enter a unit string, eg m/s; time type: enter a strftime format string, eg %Y-%m-%d %H:%M:%S, or %Y-Q%Q"
        ttk::combobox .wiring.context.axisMenu.dim.units.value
        dimFormatPopdown .wiring.context.axisMenu.dim.units.value [minsky.canvas.item.dimensionType] {}
        pack .wiring.context.axisMenu.dim.units.label .wiring.context.axisMenu.dim.units.value -side left
        pack .wiring.context.axisMenu.dim.type .wiring.context.axisMenu.dim.units
        buttonBar .wiring.context.axisMenu.dim {
            minsky.canvas.item.setDimension [.wiring.context.axisMenu.dim.type.value get] [.wiring.context.axisMenu.dim.units.value get]
        }
    } else {
        deiconify .wiring.context.axisMenu.dim
    }
    .wiring.context.axisMenu.dim.type.value set [minsky.canvas.item.dimensionType]
    .wiring.context.axisMenu.dim.units.value delete 0 end
    .wiring.context.axisMenu.dim.units.value insert 0 [minsky.canvas.item.dimensionUnitsFormat]
    ensureWindowVisible .wiring.context.axisMenu.dim
    grab set .wiring.context.axisMenu.dim
    wm transient .wiring.context.axisMenu.dim
}

proc setupPickMenu {} {
    global labelPicked pickHandle
    if {![winfo exists .wiring.context.axisMenu.pick]} {
        toplevel .wiring.context.axisMenu.pick
        wm title .wiring.context.axisMenu.pick "Pick slices"
        frame .wiring.context.axisMenu.pick.select
        scrollbar .wiring.context.axisMenu.pick.select.vscroll -orient vertical -command {
            .wiring.context.axisMenu.pick.select.lb yview}
        listbox .wiring.context.axisMenu.pick.select.lb -listvariable labelPicked \
            -selectmode extended -selectforeground blue \
            -width 35 \
            -yscrollcommand {.wiring.context.axisMenu.pick.select.vscroll set} 
        pack .wiring.context.axisMenu.pick.select.lb -fill both  -expand y -side left
        pack .wiring.context.axisMenu.pick.select.vscroll -fill y -expand y -side left
        pack .wiring.context.axisMenu.pick.select
        buttonBar .wiring.context.axisMenu.pick {
            set pick {}
            foreach i [.wiring.context.axisMenu.pick.select.lb curselection] {
                lappend pick [lindex $labelPicked $i]
            }
            minsky.canvas.item.pickSliceLabels $pickHandle $pick
            minsky.canvas.item.broadcastStateToLockGroup
            reset
        }
        button .wiring.context.axisMenu.pick.buttonBar.all -text "All" -command {
            .wiring.context.axisMenu.pick.select.lb selection set 0 end}
        button .wiring.context.axisMenu.pick.buttonBar.clear -text "Clear" -command {
            .wiring.context.axisMenu.pick.select.lb selection clear 0 end}
        pack .wiring.context.axisMenu.pick.buttonBar.all .wiring.context.axisMenu.pick.buttonBar.clear -side left
    } else {
        deiconify .wiring.context.axisMenu.pick
    }
        
    set labelPicked [minsky.canvas.item.allSliceLabels]
    for {set i 0} {$i<[llength $labelPicked]} {incr i} {
        set idx([lindex $labelPicked $i]) $i
    }
    foreach i [minsky.canvas.item.pickedSliceLabels] {
        .wiring.context.axisMenu.pick.select.lb selection set $idx($i)
    }
    set pickHandle [minsky.canvas.item.selectedHandle]
    wm transient .wiring.context.axisMenu.pick
    wm geometry .wiring.context.axisMenu.pick +[winfo pointerx .]+[winfo pointery .]
    ensureWindowVisible .wiring.context.axisMenu.pick
    grab set .wiring.context.axisMenu.pick
}

