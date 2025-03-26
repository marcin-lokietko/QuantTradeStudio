import { Component, ViewChild } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { MatTableModule } from '@angular/material/table';
import { MatSort, MatSortModule } from '@angular/material/sort';
import { MatTableDataSource } from '@angular/material/table';

export interface Balance {
  asset: string;
  amountFree: string;
  usdtValue: string;
}

@Component({
  selector: 'app-wallet-page',
  imports: [MatTableModule, MatSortModule, MatIconModule],
  templateUrl: './wallet-page.component.html',
  styleUrls: ['./wallet-page.component.scss']
})
export class WalletPage {
  public balances: Balance[] = [];
  public displayedColumns: string[] = ['asset', 'amountFree', 'usdtValue'];

  dataSource = new MatTableDataSource(this.balances);
  @ViewChild(MatSort) sort!: MatSort;

  ngOnInit(): void {
    this.getData();
  }

  getData(): void {
    fetch('http://localhost:5000/wallet')
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
          console.error("Invalid wallet content:", error);
        }
        console.log('API Response:', JSON.stringify(data.balances));
      })
      .catch(error => {
        console.error('Error fetching data:', error);
      });
  }
}
