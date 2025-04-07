import { ChangeDetectorRef, Component } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { MatTableModule } from '@angular/material/table';
import { MatSortModule } from '@angular/material/sort';
import { MatButtonModule } from '@angular/material/button';
import { MatMenuModule } from '@angular/material/menu';
import { MatDialogModule } from '@angular/material/dialog';

@Component({
  selector: 'app-orders-page',
  imports: [MatTableModule, MatSortModule, MatIconModule, MatButtonModule, MatMenuModule, MatDialogModule],
  templateUrl: './orders-page.component.html',
  styleUrls: ['./orders-page.component.scss']
})
export class OrdersPage {
  public orders = '';

  constructor(private cdr: ChangeDetectorRef) {}

  ngOnInit(): void {
    this.getData();
  }

  getData(): void {
    fetch('http://localhost:5000/openOrders')
      .then(response => {
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.text();
      })
      .then(data => {
        try {
          this.orders = data;
          console.log(data)
          this.cdr.markForCheck();
        } catch (error) {
          console.error("Invalid orders content:", error);
        }
      })
      .catch(error => {
        console.error('Error fetching data:', error);
      });
  }
}
