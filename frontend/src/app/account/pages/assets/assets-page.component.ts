import { Component, ViewChild } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { MatTableModule } from '@angular/material/table';
import { MatSort, MatSortModule } from '@angular/material/sort';
import { MatTableDataSource } from '@angular/material/table';

export interface Balance {
  assetSymbol: string;
  freeAmount: string;
  usdtValue: string;
}

@Component({
  selector: 'app-assets-page',
  imports: [MatTableModule, MatSortModule, MatIconModule],
  templateUrl: './assets-page.component.html',
  styleUrls: ['./assets-page.component.scss']
})
export class AssetsPage {
  public balances: Balance[] = [];
  public displayedColumns: string[] = ['assetSymbol', 'freeAmount', 'usdtValue'];

  dataSource = new MatTableDataSource(this.balances);
  @ViewChild(MatSort) sort!: MatSort;

  ngOnInit(): void {
    this.getData();
  }

  getData(): void {
    fetch('http://localhost:5000/assets')
      .then(response => {
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.json();
      })
      .then(data => {
        try {
          this.balances = data as Balance[];
          this.dataSource = new MatTableDataSource(this.balances);
          this.dataSource.sort = this.sort;
        } catch (error) {
          console.error("Invalid assets content:", error);
        }
        console.log('API Response:', JSON.stringify(data.balances));
      })
      .catch(error => {
        console.error('Error fetching data:', error);
      });
  }
}
