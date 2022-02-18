<?php

//fetch data

global $user;
if (!array_intersect(['Vice head of institution', 'administrator'], $user->roles)) {
    die();
}

$nodes = node_load_multiple(array(), array('type' => 'project_proposal'));

$years = array();
$i = 1;
do {
    $years[] = arg($i);
    $i++;
} while (arg($i));


$table = array();
$table['cols'] = array(
    array('label' => 'Year', 'type' => 'string'),
    array('label' => 'Created', 'type' => 'number'),
    array('label' => 'Ok from Economy & VC', 'type' => 'number'),
    array('label' => 'Sent to Registrator', 'type' => 'number'),
    array('label' => 'Funded', 'type' => 'number')
);

$table_units = array(); // 1st - created, 2nd - funded
$table_units['cols'] = array(
    array('label' => 'Year', 'type' => 'string'),
    array('label' => 'eGovLab', 'type' => 'number'),
    array('label' => 'IDEAL', 'type' => 'number'),
    array('label' => 'SAS', 'type' => 'number'),
    array('label' => 'IS', 'type' => 'number'),
    array('label' => 'ACT', 'type' => 'number'),
    array('label' => 'SPIDER', 'type' => 'number')
);

$table_funds = array();
$table_funds['cols'] = array(
    array('label' => 'Year', 'type' => 'string'),
    array('label' => 'Funds requested (M Sek)', 'type' => 'number'),
    array('label' => 'Funds granted (M Sek)', 'type' => 'number')
);

$rows = array();
$funded_by_year = array();
$created_by_year = array();
$submitted_by_year = array();
$submitted_and_approved = array();
$funds_asked = array();
$funds_granted = array();
$funds_granted_unit = array();
//$unit = array();
$byarea = array();
$areas = array();
$amountarea = array();
$byunit = array();
$amountunit = array();

$unitnamefromid = array(
    247 => 'IDEAL', // Tessy former
    128 => 'IDEAL', // Harko
    372 => 'SPIDER', // Kerstin former
    416 => 'SPIDER', // John O
    65 => 'ACT', // Peter I-A
    256 => 'SAS', // Tony L
    344 => 'eGovLab', // Vasilis
    191 => 'eGovLab', // Aron since late 2019 together with Vasilis, before was SAS unti head
    170 => 'IS', // JZ former
    174 => 'IS', // Erik P former, before -2021
    227 => 'IS' // Janis since 2022
);
$a = array();

/*
foreach ($nodes as $nid => $node) {
    $deadline = $node->field_deadline[LANGUAGE_NONE][0]['value'];
    $unitid = $node->field_unit_head[LANGUAGE_NONE][0]['uid'];
    $unithead = user_load($unitid);
    if (($unitid == 191 || $unitid == 344) && $node->field_ok_from_dsv_economy[LANGUAGE_NONE][0]['value'] && $node->field_ok_from_uno[LANGUAGE_NONE][0]['value']) {
        $tempuser = user_load($node->field_dsv_person_in_charge['und'][0]['uid']);
        var_dump($node->field_start_date['und'][0]['value']);
        print date('d/m/Y', $node->field_start_date['und'][0]['date']) . ' ' . $unithead->realname . ' '.$node->title.' ('.$tempuser->realname.'): <b>'.$node->field_total_budget_amount_for_ds['und'][0]['value'].' '.$node->field_currency['und'][0]['value'].'</b><br>';
    }
}*/

print "<h3>Approved funding in " . implode(', ', $years) . "</h3>";
foreach ($nodes as $nid => $node) {
    $deadline = $node->field_deadline[LANGUAGE_NONE][0]['value'];
    $start = $node->field_start_date[LANGUAGE_NONE][0]['value'];
    $unitid = $node->field_unit_head[LANGUAGE_NONE][0]['uid'];
    // Group proposals by start year
    $year = date('Y', strtotime($start));
    // $year = date('Y', $node->created);
    if (!in_array($year, $years) or $node->created < 1489653038) {
        continue;
    }
    if ($node->field_cancelled[LANGUAGE_NONE][0]['value']) {
        continue;
    }
    if (/*$node->field_approved_funding[LANGUAGE_NONE][0]['value'] &&*/
        $node->field_ok_from_dsv_economy[LANGUAGE_NONE][0]['value'] &&
        $node->field_ok_from_uno[LANGUAGE_NONE][0]['value']) {

        $area = taxonomy_term_load($node->field_research_areas[LANGUAGE_NONE][0]['tid'])->name;
        if (!in_array($area, $areas)) {
            $areas[] = $area;
        }
        $byarea[$year][$area]++;
        $unit = '';
        // Handle aron since he used to be a SAS unit head
        if ($unitid == 191) {
            if ($year <= 2019) {
                $unit = 'SAS';
            } else if ($year >= 2020) {
                $unit = 'eGovLab';
            }
        } else {
            $unit = $unitnamefromid[$unitid];
        }
        $byunit[$year][$unit]++;

        if ($node->field_approved_funding[LANGUAGE_NONE][0]['value']) {
            $funded_by_year[$year] += 1;
            if ($node->field_currency['und'][0]['value'] == '€' || $node->field_currency['und'][0]['value'] == '$') {
                $funds_granted[$year] += $node->field_total_budget_amount_for_ds['und'][0]['value'] * 10;
                $funds_granted_unit[$year][$unit] += $node->field_total_budget_amount_for_ds['und'][0]['value'] * 10;
            } elseif ($node->field_currency['und'][0]['value'] == 'kr') {
                $funds_granted[$year] += $node->field_total_budget_amount_for_ds['und'][0]['value'];
                $funds_granted_unit[$year][$unit] += $node->field_total_budget_amount_for_ds['und'][0]['value'];
            }
            $tempuser = user_load($node->field_dsv_person_in_charge['und'][0]['uid']);
            print $year . ' ' . $node->title . ' (' . $tempuser->realname . '): <b>' . $node->field_total_budget_amount_for_ds['und'][0]['value'] . ' ' . $node->field_currency['und'][0]['value'] . '</b><br>';
        }

        if ($node->field_currency['und'][0]['value'] == '€' || $node->field_currency['und'][0]['value'] == '$') {
            $amountarea[$year][$area] += $node->field_total_budget_amount_for_ds['und'][0]['value'] * 10;
            $amountunit[$year][$unit] += $node->field_total_budget_amount_for_ds['und'][0]['value'] * 10;
        } elseif ($node->field_currency['und'][0]['value'] == 'kr') {
            $amountarea[$year][$area] += $node->field_total_budget_amount_for_ds['und'][0]['value'];
            $amountunit[$year][$unit] += $node->field_total_budget_amount_for_ds['und'][0]['value'];
        }
    }

    if ($node->field_sent_to_birgitta_o[LANGUAGE_NONE][0]['value']) {
        $submitted_by_year[$year] += 1;
    }

    if ($node->field_ok_from_dsv_economy[LANGUAGE_NONE][0]['value'] && $node->field_ok_from_uno[LANGUAGE_NONE][0]['value']) {
        $submitted_and_approved[$year] += 1;
        if ($node->field_currency['und'][0]['value'] == '€' || $node->field_currency['und'][0]['value'] == '$') {
            $funds_asked[$year] += $node->field_total_budget_amount_for_ds['und'][0]['value'] * 10;
        } elseif ($node->field_currency['und'][0]['value'] == 'kr') {
            $funds_asked[$year] += $node->field_total_budget_amount_for_ds['und'][0]['value'];
        }
    }
    $created_by_year[$year] += 1;
    $a[] = $unitid;
}


print "<h3>Proposals with OK from Economy & VC, categorised by research areas</h3>";
foreach ($areas as $area) {
    print "Area: $area</br>";
    foreach ($years as $year) {
        if ($byarea[$year][$area]) {
            $n = $byarea[$year][$area];
            $m = $amountarea[$year][$area];
            print "$year: <b>$n</b> proposals with a total budget of <b>" . number_format($m) . "</b> SEK</br>";
        }
    }
    print "<br>";
}

print "<h3>Overall for DSV</h3>";
foreach ($years as $year) {
    print "Total approved in <b>$year</b>: <b>" . array_sum($byunit[$year]) . "</b> proposals and <b>" . number_format(array_sum($amountunit[$year])) . "</b> kr";
    print "<br>Total granted in <b>$year</b>: <b>" . $funded_by_year[$year] . "</b> proposals and <b>" . number_format($funds_granted[$year]) . "</b> kr";
    print "<br/><br/>";
}

print "</br></br>";
print "<h3>Units comparison</h3>";
foreach (array_unique($unitnamefromid) as $unit) {
    print "Unit: $unit <br>";
    foreach ($years as $year) {
        $n = $byunit[$year][$unit];
        $b = $amountunit[$year][$unit];
        $g = $funds_granted_unit[$year][$unit];
        if ($n) {
            print "$year: <b>$n</b> proposals with a total asked funding of <b>" . number_format($b) . "</b> SEK and total granted funding of <b>" . number_format($g) . " SEK</b><br>";
        }
    }
    print "<br>";
}

$a = array_unique($a);
//var_dump($a);
ksort($created_by_year);

foreach ($created_by_year as $year => $value) {
    //  if ($year <> 2019) {continue;}
    $temp = array();
    $temp[] = array('v' => (int)$year);
    $temp[] = array('v' => (int)$created_by_year[$year]);
    $temp[] = array('v' => (int)$submitted_and_approved[$year]);
    $temp[] = array('v' => (int)$submitted_by_year[$year]);
    $temp[] = array('v' => (int)$funded_by_year[$year]);
    $rows[] = array('c' => $temp);
}

$table['rows'] = $rows;
$jsonTable = json_encode($table);

$rows = array();
foreach ($funded_by_year as $year => $value) {
    print($funds_asked[$year] . 'kr<br>');
    print($funds_granted[$year] . 'kr<br><br>');
    $temp = array();
    $temp[] = array('v' => (int)$year);
    $temp[] = array('v' => (int)$funds_asked[$year] / 1000000);
    $temp[] = array('v' => (int)$funds_granted[$year] / 1000000);
    $rows[] = array('c' => $temp);
}

$table_funds['rows'] = $rows;
$jsonTable_funds = json_encode($table_funds);

foreach ($created_by_year as $year => $value) {
    $temp = array();
    $temp[] = array('v' => (int)$year);
    $temp[] = array('v' => (int)$byunit[$year]['eGovLab']);
    $temp[] = array('v' => (int)$byunit[$year]['IDEAL']);
    $temp[] = array('v' => (int)$byunit[$year]['SAS']);
    $temp[] = array('v' => (int)$byunit[$year]['IS']);
    $temp[] = array('v' => (int)$byunit[$year]['ACT']);
    $temp[] = array('v' => (int)$byunit[$year]['SPIDER']);
    $rows_units[] = array('c' => $temp);
}

$table_units['rows'] = $rows_units;
$jsonTable_units = json_encode($table_units);


?>

<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
<script type="text/javascript">
    google.charts.load('current', {'packages': ['corechart']});
    google.charts.setOnLoadCallback(drawChart);

    function drawChart() {

        var options = {
            //title: 'Company Performance',
            // curveType: 'function',
            pieHole: 0.4,
            legend: {position: 'bottom'}
        };

        var data = new google.visualization.DataTable(<?php echo $jsonTable_funds?>);

        var view = new google.visualization.DataView(data);
        view.setColumns([0, 1,
            {
                calc: "stringify",
                sourceColumn: 1,
                type: "string",
                role: "annotation"
            },
            2, {
                calc: "stringify",
                sourceColumn: 2,
                type: "string",
                role: "annotation"
            }]);

        var chart = new google.visualization.ColumnChart(document.getElementById('bar_funding'));
        chart.draw(view, options);

        var data = new google.visualization.DataTable(<?php echo $jsonTable?>);
        var view = new google.visualization.DataView(data);
        view.setColumns([0, 1,
            {
                calc: "stringify",
                sourceColumn: 1,
                type: "string",
                role: "annotation"
            },
            2, {
                calc: "stringify",
                sourceColumn: 2,
                type: "string",
                role: "annotation"
            },
            3, {
                calc: "stringify",
                sourceColumn: 3,
                type: "string",
                role: "annotation"
            },
            4, {
                calc: "stringify",
                sourceColumn: 4,
                type: "string",
                role: "annotation"
            }]);
        var chart = new google.visualization.BarChart(document.getElementById('bar_chart'));
        chart.draw(view, options);

        var data = new google.visualization.DataTable(<?php echo $jsonTable_units?>);
        var chart = new google.visualization.ColumnChart(document.getElementById('pie_chart'));
        chart.draw(data, options);

    }
</script>
<h2>
    <center>Funding requested and approved</center>
</h2>
<div id="bar_funding" style="width: 900px;  margin: auto; height: 500px;"></div>
<h2>
    <center>Proposals per year (created with start date, approved, submitted, funded)</center>
</h2>
<div id="bar_chart" style="width: 900px; margin: auto; height: 500px;"></div>
<h2>
    <center>Proposals created & approved by Unit</center>
</h2>
<div id="pie_chart" style="width: 1000px; margin: auto; height: 500px;"></div>

<?php print render($content['links']); ?>

<?php print render($content['comments']); ?>

</div>
