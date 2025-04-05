import { ChangeDetectorRef, Component, ViewChild } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { MatTableModule } from '@angular/material/table';
import { MatSort, MatSortModule } from '@angular/material/sort';
import { MatTableDataSource } from '@angular/material/table';
import { MatButtonModule } from '@angular/material/button';
import { MatMenuModule } from '@angular/material/menu';
import { MatDialog, MatDialogModule } from '@angular/material/dialog';
import { Dialog } from '../../../common/dialog/dialog.component';

export interface Balance {
  assetSymbol: string;
  freeQuantity: string;
  usdtValue: string;
}

@Component({
  selector: 'app-assets-page',
  imports: [MatTableModule, MatSortModule, MatIconModule, MatButtonModule, MatMenuModule, MatDialogModule],
  templateUrl: './assets-page.component.html',
  styleUrls: ['./assets-page.component.scss']
})
export class AssetsPage {
  public balances: Balance[] = [];
  public displayedColumns: string[] = ['assetSymbol', 'freeQuantity', 'usdtValue'];

  dataSource = new MatTableDataSource(this.balances);
  @ViewChild(MatSort) sort!: MatSort;

  constructor(private dialog: MatDialog, private cdr: ChangeDetectorRef) {}

  ngOnInit(): void {
    this.getData();
  }

  showBuyDialog(element: any): void {
    let usdtPrice = '';

    const dialogRef = this.dialog.open(Dialog, {
      width: '40%',
      data: { title: 'Buy: ' + element.assetSymbol, details: `Unit price: ${usdtPrice} USDT` }
    });

    dialogRef.afterClosed().subscribe(result => {
      console.log('Dialog closed with result:', result);
      if (result) {
        this.buyAsset(element.assetSymbol, 'USDT', '0.001');
      }
    });
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
          this.cdr.markForCheck();
        } catch (error) {
          console.error("Invalid assets content:", error);
        }
        console.log('API Response:', JSON.stringify(data.balances));
      })
      .catch(error => {
        console.error('Error fetching data:', error);
      });
  }

  buyAsset(assetToBuy: string, assetToSpend: string, quantityToBuy: string) : void
  {
    console.log('Buying ' + quantityToBuy + ' ' + assetToBuy + ' for ' + assetToSpend );

    fetch('http://localhost:5000/makeOrder', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Access-Control-Allow-Headers':'*'
      },
      body: JSON.stringify({assetToBuy, assetToSpend, quantityToBuy})
    })
    .then(response => {
      if (!response.ok) {
        throw new Error('makeOrder HTTP error ' + response.status);
      }
      return response.json();
    })
    .then(data => {
      console.log('makeOrder successful:', data);
      this.getData();
    })
    .catch(error => {
      console.error('makeOrder failed:', error);
    });
  } 
}
